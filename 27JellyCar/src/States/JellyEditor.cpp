#include <engine/Fontrenderer.h>
#include <filesystem>
#include "JellyEditor.h"

#include "Application.h"
#include "Globals.h"
#include "JellyHelper.h"
#include "TileSet.h"
#include "SceneManager.h"


JellyEditor::JellyEditor(StateMachine& machine) : State(machine, States::JELLYEDITOR) {

	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);

	m_backWidth = Globals::textureManager.get("paper").getWidth();
	m_backHeight = Globals::textureManager.get("paper").getHeight();

	m_columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(m_backWidth));
	m_rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(m_backHeight));

	SceneManager::Get().getScene("scene").loadSettings("JellyAudioSettings.xml");
	JellyHellper::Instance()->LoadShaders();
	m_world = new World();

	SceneManager::Get().getScene("scene").configure("Assets/Jelly/Scenes_compiled_new/", true);

	Scene::InitPhysic(m_world);
	SceneManager::Get().getScene("scene").loadCarSkins("Assets/Jelly/car_skins.xml");
	SceneManager::Get().getScene("scene").loadLevel("intro.scene");
	SceneManager::Get().getScene("scene").buildLevel(m_world, m_gameBodies);
	SceneManager::Get().getScene("scene").buildCar(m_world, m_car, "Assets/Jelly/car_and_truck.car");

	const SkinInfo& skinInfo = SceneManager::Get().getScene("scene").getCurrentSkinInfo();
	m_car->SetChassisTextures(skinInfo.skinTexture.chassisSmall, skinInfo.skinTexture.chassisBig);
	m_car->SetTireTextures(skinInfo.skinTexture.tireSmall, skinInfo.skinTexture.tireBig);

	m_jellyProjection = Matrix4f::Orthographic(-20.0f + 0, 0 + 20.0f, -4.2f + 4, 4 + 18.2f, -1.0f, 1.0f);
	m_levelTarget = SceneManager::Get().getScene("scene").getLevelTarget();

	Globals::textureManager.get("paper").bind(1);
	Globals::textureManager.get("logo").bind(2);
	Texture::SetActivateTexture(0);

	auto shader = Globals::shaderManager.getAssetPointer("quad_back");
	shader->use();
	shader->loadInt("u_texture", 1);
	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	shader->loadInt("u_texture", 2);
	shader->unuse();

	//ground shape and floor
	ClosedShape groundShape;
	groundShape.begin();
	groundShape.addVertex(Vector2(-15.0f, -1.0f));
	groundShape.addVertex(Vector2(-15.0f, 1.0f));
	groundShape.addVertex(Vector2(15.0f, 1.0f));
	groundShape.addVertex(Vector2(15.0f, -1.0f));
	groundShape.finish();

	groundBody = new StaticBody(m_world, groundShape, 0, Vector2(0.0f, -10.0f), 0, Vector2::One, false);

	//falling shape
	fallingShape.begin();
	fallingShape.addVertex(Vector2(-1.0f, 0.0f));
	fallingShape.addVertex(Vector2(0.0f, 1.0f));
	fallingShape.addVertex(Vector2(1.0f, 0.0f));
	fallingShape.addVertex(Vector2(0.0f, -1.0f));
	fallingShape.finish();

	//pressure shape
	pressureShape.begin();
	for (int i = 0; i < 360; i += 20){
		pressureShape.addVertex(Vector2(cosf(VectorTools::degToRad((float)-i)), sinf(VectorTools::degToRad((float)-i))));
	}
	pressureShape.finish();

	//spring shape
	springShape.begin();
	springShape.addVertex(Vector2(-1.5f, 2.0f));
	springShape.addVertex(Vector2(-0.5f, 2.0f));
	springShape.addVertex(Vector2(0.5f, 2.0f));
	springShape.addVertex(Vector2(1.5f, 2.0f));
	springShape.addVertex(Vector2(1.5f, 1.0f));
	springShape.addVertex(Vector2(0.5f, 1.0f));
	springShape.addVertex(Vector2(0.5f, -1.0f));
	springShape.addVertex(Vector2(1.5f, -1.0f));
	springShape.addVertex(Vector2(1.5f, -2.0f));
	springShape.addVertex(Vector2(0.5f, -2.0f));
	springShape.addVertex(Vector2(-0.5f, -2.0f));
	springShape.addVertex(Vector2(-1.5f, -2.0f));
	springShape.addVertex(Vector2(-1.5f, -1.0f));
	springShape.addVertex(Vector2(-0.5f, -1.0f));
	springShape.addVertex(Vector2(-0.5f, 1.0f));
	springShape.addVertex(Vector2(-1.5f, 1.0f));
	springShape.finish();

	//important!!!!
	m_world->setWorldLimits(Vector2(-100.0f, -100.0f), Vector2(100.0f, 100.0f));

	m_screenBounds = Vector4f(-20.0f + 0, 0 + 20.0f, -4.2f - 5, -5 + 18.2f);
}

JellyEditor::~JellyEditor() {
	EventDispatcher::RemoveMouseListener(this);
	EventDispatcher::RemoveKeyboardListener(this);

	Scene::ClearLevel(m_world, m_gameBodies, m_car);
}

void JellyEditor::fixedUpdate() {}

void JellyEditor::update() {

	processInput();

	m_car->setTorque(1);

	//Update physic
	for (int i = 0; i < 6; i++) {
		m_world->update(0.004f);

		for (size_t i = 0; i < m_gameBodies.size(); i++)
			m_gameBodies[i]->Update(0.004f);

		m_car->clearForces();
		m_car->update(0.004f);
	}

	//reset car position
	if (m_car->getPosition().Y < SceneManager::Get().getScene("scene").getLevelLine()) {
		Vector2 pos = SceneManager::Get().getScene("scene").getCarStartPos();
		Vector2 scale = Vector2(1.0f, 1.0f);

		m_car->getChassisBody()->setPositionAngle(pos, 0.0f, scale);
		m_car->getTire(0)->setPositionAngle(pos, 0.0f, scale);
		m_car->getTire(1)->setPositionAngle(pos, 0.0f, scale);
	}

	bool enterMenu = false;

	//end of scene
	if (m_car->getChassisBody()->getAABB().contains(m_levelTarget)) {
		if (m_car->getChassisBody()->contains(m_levelTarget)) {
			enterMenu = true;
		}
	}

	for (int i = 0; i < 2; i++) {
		if (m_car->getTire(i)->getAABB().contains(m_levelTarget)) {
			if (m_car->getTire(i)->contains(m_levelTarget)) {
				enterMenu = true;
			}
		}
	}

	if (enterMenu) {
		auto shader = Globals::shaderManager.getAssetPointer("quad_back");
		shader->use();
		shader->loadInt("u_texture", 0);
		shader->unuse();

		shader = Globals::shaderManager.getAssetPointer("quad");
		shader->use();
		shader->loadInt("u_texture", 0);
		shader->unuse();

		m_isRunning = false;
		m_machine.addStateAtBottom(new JellyEditor(m_machine));
	}
}

void JellyEditor::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto shader = Globals::shaderManager.getAssetPointer("quad_back");

	shader->use();
	for (int y = 0; y < m_rows; y++) {
		for (int x = 0; x < m_columns; x++) {

			int posx = m_backWidth * x;
			int posy = Application::Height - m_backHeight * (1 + y);

			shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(posx), static_cast<float>(posy), 0.0f) * Matrix4f::Scale(static_cast<float>(m_backWidth), static_cast<float>(m_backHeight), 1.0f));
			Globals::shapeManager.get("quad_aligned").drawRaw();
		}
	}

	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();

	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(Application::Width / 2), static_cast<float>(Application::Height - 200), 0.0f) * Matrix4f::Scale(static_cast<float>(Globals::textureManager.get("logo").getWidth()), static_cast<float>(Globals::textureManager.get("logo").getHeight()), 1.0f));
	Globals::shapeManager.get("quad_half").drawRaw();
	shader->unuse();

	for (size_t i = 0; i < m_gameBodies.size(); i++) {
		m_gameBodies[i]->Draw(m_jellyProjection);
	}

	m_car->Draw(m_jellyProjection);

	//draw ground
	RenderGlobalShapeLine(groundBody->mPointMasses, 1.0f, 1.0f, 0.0f);

	//bodies
	for (unsigned int i = 0; i < fallingBodies.size(); i++)
		RenderGlobalShapeLine(fallingBodies[i]->mPointMasses, 1.0f, 1.0f, 0.0f);

	//pessure bodies
	for (unsigned int i = 0; i < pressureBodies.size(); i++)
		RenderGlobalShapeLine(pressureBodies[i]->mPointMasses, 1.0f, 1.0f, 0.0f);

	//spring bodies
	for (unsigned int i = 0; i < springBodies.size(); i++)
		RenderGlobalShapeLine(springBodies[i]->mPointMasses, 1.0f, 1.0f, 0.0f);

	if (dragBody != NULL){
		RenderLine(dragBody->getPointMass(dragPoint)->Position, Vector2(dragX, dragY), 1, 1, 1);
	}
}

void JellyEditor::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void JellyEditor::resize(int deltaW, int deltaH) {
	m_columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(m_backWidth));
	m_rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(m_backHeight));
}

int scaleX = 20;
int scaleY = 20;

int moveX = 960 / 2;
int moveY = 544 / 2;

void JellyEditor::RenderGlobalShapeLine(std::vector<PointMass> &mPointMasses, float R, float G, float B){
	for (unsigned int i = 0; i < mPointMasses.size(); i++){
		if ((i + 1) < mPointMasses.size()){
			int pointX1 = (mPointMasses[i].Position.X * scaleX) + moveX;
			int pointX2 = (mPointMasses[i + 1].Position.X * scaleX) + moveX;

			int pointY1 = (544 - (mPointMasses[i].Position.Y * scaleY)) - moveY;
			int pointY2 = (544 - (mPointMasses[i + 1].Position.Y * scaleY)) - moveY;

			//vita2d_draw_line(pointX1, pointY1, pointX2, pointY2, RGBA8((int)(R * 255.0f), (int)(G * 255.0f), (int)(B * 255.0f), 255));
		}
	}

	int pointX1 = (mPointMasses[mPointMasses.size() - 1].Position.X * scaleX) + moveX;
	int pointX2 = (mPointMasses[0].Position.X * scaleX) + moveX;

	int pointY1 = (544 - (mPointMasses[mPointMasses.size() - 1].Position.Y * scaleY)) - moveY;
	int pointY2 = (544 - (mPointMasses[0].Position.Y * scaleY)) - moveY;

	//vita2d_draw_line(pointX1, pointY1, pointX2, pointY2, RGBA8((int)(R * 255.0f), (int)(G * 255.0f), (int)(B * 255.0f), 255));
}

void JellyEditor::RenderLine(Vector2 start, Vector2 stop, float R, float G, float B){
	int pointX1 = (start.X * scaleX) + moveX;
	int pointX2 = (stop.X * scaleX) + moveX;

	int pointY1 = (544 - (start.Y * scaleY)) - moveY;
	int pointY2 = (544 - (stop.Y * scaleY)) - moveY;

	//vita2d_draw_line(pointX1, pointY1, pointX2, pointY2, RGBA8((int)(R * 255.0f), (int)(G * 255.0f), (int)(B * 255.0f), 255));
}

Vector2f JellyEditor::touchToScreen(Vector4f screenBound, Vector2f touch) {
	float width = fabsf(screenBound[0]) + fabsf(screenBound[1]);
	float widthFactor = width / static_cast<float>(Application::Width);
	float dragX = (touch[0] * widthFactor) + screenBound[0];

	float height = fabsf(screenBound[3]) + fabsf(screenBound[2]);
	float heightFactor = height / static_cast<float>(Application::Height);

	float dragY = 0.0f;

	if (screenBound[3] < screenBound[2])
		dragY = (touch[1] * heightFactor) + screenBound[3];
	else {
		touch[1] = static_cast<float>(Application::Height) - touch[1];
		dragY = (touch[1] * heightFactor) + screenBound[2];
	}

	return Vector2f(dragX, dragY);
}

void JellyEditor::processInput() {
	Keyboard &keyboard = Keyboard::instance();
	Mouse &mouse = Mouse::instance();

	if (keyboard.keyPressed(Keyboard::KEY_A)){
		if (fall){
			FallingBody* body = new FallingBody(m_world, fallingShape, 1.0f, 300.0f, 10.0f, Vector2(-2.0f, 5.0f), VectorTools::degToRad(rand() % 360), Vector2::One);

			body->addInternalSpring(0, 2, 400.0f, 12.0f);
			body->addInternalSpring(1, 3, 400.0f, 12.0f);

			fallingBodies.push_back(body);
			fall = false;
		}
	}else{
		fall = true;
	}

	if (keyboard.keyPressed(Keyboard::KEY_S)){
		if (pressure){
			GamePressureBody * pressureBody = new GamePressureBody(m_world, pressureShape, 1.0f, 70.0f, 10.0f, 1.0f, 300.0f, 20.0f, Vector2(2, 5), 0, Vector2(0.5f, 0.5f), false);
			pressureBodies.push_back(pressureBody);

			pressure = false;
		}
	}else{
		pressure = true;
	}

	if (keyboard.keyPressed(Keyboard::KEY_D)){
		if (spring){
			GameSpringBody *springBody = new GameSpringBody(m_world, springShape, 1.0f, 150.0f, 5.0f, 300.0f, 15.0f, Vector2(4, 5), 0.0f, Vector2::One, false);

			springBody->addInternalSpring(0, 14, 300.0f, 10.0f);
			springBody->addInternalSpring(1, 14, 300.0f, 10.0f);
			springBody->addInternalSpring(1, 15, 300.0f, 10.0f);
			springBody->addInternalSpring(1, 5, 300.0f, 10.0f);
			springBody->addInternalSpring(2, 14, 300.0f, 10.0f);
			springBody->addInternalSpring(2, 5, 300.0f, 10.0f);
			springBody->addInternalSpring(1, 5, 300.0f, 10.0f);
			springBody->addInternalSpring(14, 5, 300.0f, 10.0f);
			springBody->addInternalSpring(2, 4, 300.0f, 10.0f);
			springBody->addInternalSpring(3, 5, 300.0f, 10.0f);
			springBody->addInternalSpring(14, 6, 300.0f, 10.0f);
			springBody->addInternalSpring(5, 13, 300.0f, 10.0f);
			springBody->addInternalSpring(13, 6, 300.0f, 10.0f);
			springBody->addInternalSpring(12, 10, 300.0f, 10.0f);
			springBody->addInternalSpring(13, 11, 300.0f, 10.0f);
			springBody->addInternalSpring(13, 10, 300.0f, 10.0f);
			springBody->addInternalSpring(13, 9, 300.0f, 10.0f);
			springBody->addInternalSpring(6, 10, 300.0f, 10.0f);
			springBody->addInternalSpring(6, 9, 300.0f, 10.0f);
			springBody->addInternalSpring(6, 8, 300.0f, 10.0f);
			springBody->addInternalSpring(7, 9, 300.0f, 10.0f);

			springBodies.push_back(springBody);
			spring = false;
		}
	}else{
		spring = true;
	}

	if (mouse.buttonDown(Mouse::BUTTON_LEFT)){

		Vector2f touch = touchToScreen(m_screenBounds, Vector2f(mouse.xPos(), mouse.yPos()));
		//fxTouch = (lerp(touch[0], 1919, 960) - 50);
		//fyTouch = (lerp(touch[1], 1087, 544) - 56);

		//Vector2 dragVector = Renderer::ScreenToWorld((lerp(touch[0], 1919, 960) - 50), (lerp(touch[1], 1087, 544) - 56));

		//dragX = dragVector.X;
		//dragY = dragVector.Y;

		touchF = true;
	}else{
		touchF = false;
	}

	if (touchF == true){
		if (dragBody != NULL){
			PointMass *pm = dragBody->getPointMass(dragPoint);
			dragBody->setDragForce(VectorTools::calculateSpringForce(pm->Position, pm->Velocity, Vector2(dragX, dragY), Vector2(0, 0), 0.0f, 100.0f, 10.0f), dragPoint);
		}
	}else{
		dragBody = NULL;
		dragPoint = -1;
	}

	if (touchF == true){
		if (dragBody == NULL){
			int body;
			m_world->getClosestPointMass(Vector2(dragX, dragY), body, dragPoint);
			dragBody = m_world->getBody(body);
		}
	}
}