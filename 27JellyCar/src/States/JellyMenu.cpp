#include <engine/Fontrenderer.h>

#include "JellyMenu.h"
#include "Application.h"
#include "Globals.h"
#include "TileSet.h"

#include "tinyxml.h"
#include "JellyHelper.h"
#include "JellyOptions.h"
#include "JellyGame.h"

JellyMenu::JellyMenu(StateMachine& machine) : State(machine, States::JELLYMENU),
currentPosition(SceneManager::Get().getSceneInfo("scene").m_currentPosition),
carcurrentPosition(SceneManager::Get().getSceneInfo("scene").m_carCurrentPosition) {

	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);


	m_backWidth = Globals::textureManager.get("paper").getWidth();
	m_backHeight = Globals::textureManager.get("paper").getHeight();

	m_columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(m_backWidth));
	m_rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(m_backHeight));

	m_controlsWidth = static_cast<float>(Globals::textureManager.get("controls").getWidth());
	m_controlsHeight = static_cast<float>(Globals::textureManager.get("controls").getHeight());

	JellyHellper::Instance()->LoadShaders();
	m_world = new World();

	m_levelManager = new LevelManager();
	m_levelManager->SetAssetsLocation("Assets/Jelly/");
	m_levelManager->LoadAllScenes("scene_list.xml");
	m_levelManager->LoadCarSkins("car_skins.xml");
	m_levelManager->LoadScores("JellyScore.xml");
	m_levelManager->InitPhysic(m_world);
	m_levelManager->LoadLevel(m_world, "menu.scene", "Assets/Jelly/car_and_truck.car");
	m_levelManager->LoadCompiledLevel(m_world, "menu.scene", "Assets/Jelly/car_and_truck.car");

	m_sceneFiles = m_levelManager->GetScenes();
	m_carSkins = m_levelManager->GetCarSkins();

	m_car = m_levelManager->GetCar();
	m_car->SetChassisTextures(m_levelManager->GetCarImage(m_carSkins[0].chassisSmall), m_levelManager->GetCarImage(m_carSkins[0].chassisBig));
	m_car->SetTireTextures(m_levelManager->GetCarImage(m_carSkins[0].tireSmall), m_levelManager->GetCarImage(m_carSkins[0].tireBig));

	m_gameBodies = m_levelManager->GetLevelBodies();
	m_jellyProjection = glm::ortho(-20.0f + 0, 0 + 20.0f, -4.2f + 4, 4 + 18.2f, -1.0f, 1.0f);


	SceneManager::Get().getSceneInfo("scene").loadLevelInfo("Assets/Jelly/scene_list.xml");
	SceneManager::Get().getSceneInfo("scene").loadCarSkins("Assets/Jelly/car_skins.xml");
	TileSetManager::Get().getTileSet("thumbs").loadTileSet(SceneManager::Get().getSceneInfo("scene").getThumbFiles());
	m_thumbAtlas = TileSetManager::Get().getTileSet("thumbs").getAtlas();
	//Spritesheet::Safe("thumbs", m_thumbAtlas);

	SceneManager::Get().getSceneInfo("scene").loadScores("JellyScore.xml");
}

JellyMenu::~JellyMenu() {
	EventDispatcher::RemoveMouseListener(this);
	EventDispatcher::RemoveKeyboardListener(this);

	if (m_levelManager != 0){
		//clear level
		m_levelManager->ClearLevel(m_world);

		//remove level manager
		delete m_levelManager;
	}

	//remove physic world
	delete m_world;
	m_gameBodies.clear();
}

void JellyMenu::fixedUpdate() {}

void JellyMenu::update() {

	processInput();

	for (int i = 0; i < 6; i++){
		m_world->update(0.004f);

		for (size_t i = 0; i < m_gameBodies.size(); i++)
			m_gameBodies[i]->Update(0.004f);

		m_car->clearForces();
		m_car->update(0.004f);
	}

	//reset car position
	if (m_car->getPosition().Y < m_levelManager->GetLevelLine()){
		Vector2 pos = m_levelManager->GetCarStartPos();
		Vector2 scale = Vector2(1.0f, 1.0f);

		m_car->getChassisBody()->setPositionAngle(pos, 0.0f, scale);
		m_car->getTire(0)->setPositionAngle(pos, 0.0f, scale);
		m_car->getTire(1)->setPositionAngle(pos, 0.0f, scale);
	}
}

void JellyMenu::render() {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	auto shader = Globals::shaderManager.getAssetPointer("quad_back");
	shader->use();
	Globals::textureManager.get("paper").bind(0);

	for (int y = 0; y < m_rows; y++){
		for (int x = 0; x < m_columns; x++) {

			int posx = m_backWidth * x;
			int posy = Application::Height - m_backHeight * (1 + y);

			shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(posx), static_cast<float>(posy), 0.0f) * Matrix4f::Scale(static_cast<float>(m_backWidth), static_cast<float>(m_backHeight), 1.0f));
			Globals::shapeManager.get("quad_aligned").drawRaw();
		}
	}
	Globals::textureManager.get("paper").unbind(0);

	shader->unuse();

	//menu level
	for (size_t i = 0; i < m_gameBodies.size(); i++){
		m_gameBodies[i]->Draw(m_jellyProjection);
	}

	//car
	m_car->Draw(m_jellyProjection);

	shader = Globals::shaderManager.getAssetPointer("quad_array");
	shader->use();
	Spritesheet::Bind(m_thumbAtlas);	
	const TextureRect& rect = TileSetManager::Get().getTileSet("thumbs").getTextureRects()[currentPosition];
	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(Application::Width / 2), static_cast<float>(Application::Height / 2 + rect.height / 2 + 20), 0.0f) * Matrix4f::Scale(static_cast<float>(rect.width), static_cast<float>(rect.height), 1.0f));
	shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY , rect.textureOffsetX + rect.textureWidth , rect.textureOffsetY  + rect.textureHeight));
	shader->loadInt("u_layer", rect.frame);
	Globals::shapeManager.get("quad_half").drawRaw();
	Spritesheet::Unbind();
	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();

	Globals::textureManager.get("controls").bind(0);

	int posx = Application::Width / 2 - 40 ;
	int posy = 29;
	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(posx), static_cast<float>(posy), 0.0f)* Matrix4f::Scale(static_cast<float>(74), static_cast<float>(38), 1.0f));
	shader->loadVector("u_texRect", Vector4f(239.0f / m_controlsWidth, (m_controlsHeight - (194.0f + 38.0f)) / m_controlsHeight, (239.0f + 74.0f) / m_controlsWidth, (m_controlsHeight - 194.0f) / m_controlsHeight));

	Globals::shapeManager.get("quad_half").drawRaw();

	posx = Application::Width / 2 + 40 ;
	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(posx), static_cast<float>(posy), 0.0f)* Matrix4f::Scale(static_cast<float>(74), static_cast<float>(38), 1.0f));
	shader->loadVector("u_texRect", Vector4f(159.0f/ m_controlsWidth, (512.0f - (193.0f + 38.0f)) / m_controlsHeight, (159.0f + 74.0f) / m_controlsWidth, (m_controlsHeight - 193.0f) / m_controlsHeight));

	Globals::shapeManager.get("quad_half").drawRaw();

	Globals::textureManager.get("controls").unbind(0);
	shader->unuse();

	Globals::spritesheetManager.getAssetPointer("jelly_font")->bind(0);

	int levelTextPositionY = Application::Height - ( (Application::Height / 2) - 156 - 30);

	const LevelInfo2& levelInfo = SceneManager::Get().getSceneInfo("scene").getCurrentLevelInfo();

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_64"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_64").getWidth(levelInfo.name) / 2), levelTextPositionY, levelInfo.name, Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_64"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_64").getWidth(levelInfo.name) / 2), levelTextPositionY + 3, levelInfo.name, Vector4f(1.0f,  0.65f, 0.0f, 1.0f));

	int timePositionX = (Application::Width / 2) - (272 / 2);
	int jumpPositionX = (Application::Width / 2) + (272 / 2);
	int textPosY = Application::Height - ((Application::Height / 2) + 30);

	std::string time = Fontrenderer::FloatToString(levelInfo.time, 2);
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), timePositionX , textPosY, time + "s", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), timePositionX , textPosY + 2, time + "s", Vector4f(0.0f, 0.84f, 0.0f, 1.0f));

	std::string jump = Fontrenderer::FloatToString(levelInfo.jump, 2);
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), jumpPositionX - Globals::fontManager.get("jelly_32").getWidth(jump + "m"), textPosY, jump + "m", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), jumpPositionX - Globals::fontManager.get("jelly_32").getWidth(jump + "m"), textPosY + 2, jump + "m", Vector4f(0.71f, 0.16f, 0.18f, 1.0f));

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - 90 - Globals::fontManager.get("jelly_32").getWidth("Options")), 10, "Options", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 90), 10, "Exit", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().drawBuffer();
	Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);
	

}

void JellyMenu::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		//m_isRunning = false;
	}
}

void JellyMenu::resize(int deltaW, int deltaH) {
	m_columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(m_backWidth));
	m_rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(m_backHeight));
}

void JellyMenu::processInput() {
	Keyboard &keyboard = Keyboard::instance();
	
	if (keyboard.keyPressed(Keyboard::KEY_SPACE)) {
		m_isRunning = false;
	}

	if (keyboard.keyPressed(Keyboard::KEY_ENTER) && !keyboard.keyDown(Keyboard::KEY_RALT)){

		auto shader = Globals::shaderManager.getAssetPointer("quad");
		shader->use();
		shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
		shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
		shader->unuse();

		m_machine.addStateAtTop(new JellyOptions(m_machine));
		return;
	}

	if (keyboard.keyPressed(Keyboard::KEY_S)) {
		auto shader = Globals::shaderManager.getAssetPointer("quad");
		shader->use();
		shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
		shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
		shader->unuse();

		m_isRunning = false;
		m_machine.addStateAtBottom(new JellyGame(m_machine, "scene"));
		return;
	
	}

	if (keyboard.keyPressed(Keyboard::KEY_UP)){
		currentPosition--;

		if (currentPosition < 0){
			currentPosition = 0;
		}
		//_audioHelper->PlayHitSound();
	}

	if (keyboard.keyPressed(Keyboard::KEY_DOWN)){
		currentPosition++;

		if (currentPosition >= m_sceneFiles.size()){
			currentPosition = m_sceneFiles.size() - 1;
		}
		//_audioHelper->PlayHitSound();
	}

	if (keyboard.keyPressed(Keyboard::KEY_D)){
		carcurrentPosition++;

		if (carcurrentPosition >= m_carSkins.size()){
			carcurrentPosition = m_carSkins.size() - 1;
		}

		m_car->SetChassisTextures(m_levelManager->GetCarImage(m_carSkins[carcurrentPosition].chassisSmall), m_levelManager->GetCarImage(m_carSkins[carcurrentPosition].chassisBig));
		m_car->SetTireTextures(m_levelManager->GetCarImage(m_carSkins[carcurrentPosition].tireSmall), m_levelManager->GetCarImage(m_carSkins[carcurrentPosition].tireBig));
	}

	if (keyboard.keyPressed(Keyboard::KEY_A)){
		carcurrentPosition--;

		if (carcurrentPosition < 0){
			carcurrentPosition = 0;
		}

		m_car->SetChassisTextures(m_levelManager->GetCarImage(m_carSkins[carcurrentPosition].chassisSmall), m_levelManager->GetCarImage(m_carSkins[carcurrentPosition].chassisBig));
		m_car->SetTireTextures(m_levelManager->GetCarImage(m_carSkins[carcurrentPosition].tireSmall), m_levelManager->GetCarImage(m_carSkins[carcurrentPosition].tireBig));
	}

	if (keyboard.keyPressed(Keyboard::KEY_W)){
		m_car->Transform();
	}

	if (!keyboard.keyDown(Keyboard::KEY_LEFT) && !keyboard.keyDown(Keyboard::KEY_RIGHT))
		m_car->setTorque(0);

	if (keyboard.keyDown(Keyboard::KEY_LEFT)){
		m_car->setTorque(-1);
	}

	if (keyboard.keyDown(Keyboard::KEY_RIGHT)){
		m_car->setTorque(1);
	}

	//chasis torque
	m_car->mChassis->torque = 0.0f;

	if (keyboard.keyDown(Keyboard::KEY_Q)){
		m_car->mChassis->torque = -1.0f;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)){
		m_car->mChassis->torque = 1.0f;
	}
}
