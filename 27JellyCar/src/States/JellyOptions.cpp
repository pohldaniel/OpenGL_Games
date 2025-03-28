#include <engine/Fontrenderer.h>
#include <tinyxml.h>

#include "JellyOptions.h"
#include "JellyMenu.h"
#include "Application.h"
#include "Globals.h"
#include "JellyHelper.h"
#include "TileSet.h"
#include "MusicManager.h"

JellyOptions::JellyOptions(StateMachine& machine) : State(machine, States::JELLYOPTIONS) {

	EventDispatcher::AddKeyboardListener(this);
	Mouse::instance().attach(Application::GetWindow(), false);


	m_backWidth = Globals::textureManager.get("paper").getWidth();
	m_backHeight = Globals::textureManager.get("paper").getHeight();

	m_columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(m_backWidth));
	m_rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(m_backHeight));

	m_controlsWidth = static_cast<float>(Globals::textureManager.get("controls").getWidth());
	m_controlsHeight = static_cast<float>(Globals::textureManager.get("controls").getHeight());

	JellyHellper::Instance()->LoadShaders();

	m_world = new World();

	Scene::InitPhysic(m_world);
	SceneManager::Get().getScene("scene").loadLevel("options_scene.scene");
	SceneManager::Get().getScene("scene").buildLevel(m_world, m_gameBodies);

	m_screenBounds = Vector4f(-20.0f + 0, 0 + 20.0f, -4.2f - 5, -5 + 18.2f);
	m_jellyProjection = Matrix4f::Orthographic(-20.0f + 0, 0 + 20.0f, -4.2f - 5, -5 + 18.2f, -1.0f, 1.0f);

	//TileSetManager::Get().getTileSet("options").loadTileSetCpu({
	//		"Assets/Jelly/Options/credits.png",
	//		"Assets/Jelly/Options/libs.png",
	//		"Assets/Jelly/Options/keyboard.png",
	//		"Assets/Jelly/Options/gamepad.png",
	//		"Assets/Jelly/Options/secret.png",
	//		"Assets/Jelly/Options/volume.png" });
	//
	//
	//TileSetManager::Get().getTileSet("options").loadTileSetGpu();
	//m_options = TileSetManager::Get().getTileSet("options").getAtlas();
	//Spritesheet::Safe("Options", m_options);

	//texture test
	for (size_t i = 0; i < m_gameBodies.size(); i++){
		if (m_gameBodies[i]->GetName() == "options_libs"){
			m_gameBodies[i]->SetTexture(&Globals::textureManager.get("libs"));
			//m_gameBodies[i]->SetTextureRect(TileSetManager::Get().getTileSet("options").getTextureRects()[1]);
			m_menuBodies.push_back(m_gameBodies[i]);
		}

		if (m_gameBodies[i]->GetName() == "options_credits"){
			m_gameBodies[i]->SetTexture(&Globals::textureManager.get("credits"));
			//m_gameBodies[i]->SetTextureRect(TileSetManager::Get().getTileSet("options").getTextureRects()[0]);
			m_menuBodies.push_back(m_gameBodies[i]);
		}

		if (m_gameBodies[i]->GetName() == "options_keys"){
			m_gameBodies[i]->SetTexture(&Globals::textureManager.get("gamepad"));
			//m_gameBodies[i]->SetTextureRect(TileSetManager::Get().getTileSet("options").getTextureRects()[2]);
			m_menuBodies.push_back(m_gameBodies[i]);
		}

		if (m_gameBodies[i]->GetName() == "options_sound"){
			m_gameBodies[i]->SetTexture(&Globals::textureManager.get("volume"));
			//m_gameBodies[i]->SetTextureRect(TileSetManager::Get().getTileSet("options").getTextureRects()[5]);
			m_menuBodies.push_back(m_gameBodies[i]);
		}

		if (m_gameBodies[i]->GetName() == "options_secret"){
			m_gameBodies[i]->SetTexture(&Globals::textureManager.get("secret"));
			//m_gameBodies[i]->SetTextureRect(TileSetManager::Get().getTileSet("options").getTextureRects()[4]);
			m_menuBodies.push_back(m_gameBodies[i]);
		}
	}

	m_menuBodySelected = 0;
	m_alphaScale = 1.0f;

	m_dragBody = NULL;
}

JellyOptions::~JellyOptions() {
	EventDispatcher::RemoveKeyboardListener(this);
	Scene::ClearLevel(m_world, m_gameBodies, nullptr);
}

void JellyOptions::OnKeyDown(Event::KeyboardEvent& event) {	
	if (!m_states.empty()) {
		m_states.top()->OnKeyDown(event);
	}else {
		if (event.keyCode == VK_ESCAPE) {
			State::m_isRunning = false;
		}
	}
}

void JellyOptions::resize(int deltaW, int deltaH) {
	m_columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(m_backWidth));
	m_rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(m_backHeight));

	m_machine.resizeState(deltaW, deltaH, States::JELLYGAME);
	m_machine.resizeState(deltaW, deltaH, States::JELLYMENU);

	if (!m_states.empty()) {
		m_states.top()->resize(deltaW, deltaH);
	}
}

Vector2f JellyOptions::touchToScreen(Vector4f screenBound, Vector2f touch) {
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

void JellyOptions::processInput() {
	Keyboard &keyboard = Keyboard::instance();
	Mouse &mouse = Mouse::instance();

	//keys
	if (keyboard.keyPressed(Keyboard::KEY_LEFT)){
		m_menuBodySelected--;

		if (m_menuBodySelected < 0){
			m_menuBodySelected = 0;
		}
	}

	//move down
	if (keyboard.keyPressed(Keyboard::KEY_RIGHT)){
		m_menuBodySelected++;
		if (m_menuBodySelected > m_menuBodies.size() - 1){
			m_menuBodySelected = m_menuBodies.size() - 1;
		}
	}
	
	//touch 
	if (mouse.buttonDown(Mouse::BUTTON_LEFT)) {
		Vector2f touch = touchToScreen(m_screenBounds, Vector2f(mouse.xPos(), mouse.yPos()));
		m_dragX = touch[0];
		m_dragY = touch[1];

		m_touchF = true;

	}else{
		m_touchF = false;

	}

	if (m_touchF == true){
		if (m_dragBody != NULL){
			PointMass *pm = m_dragBody->getPointMass(m_dragPoint);
			m_dragBody->setDragForce(VectorTools::calculateSpringForce(pm->Position, pm->Velocity, Vector2(m_dragX, m_dragY), Vector2(0, 0), 0.0f, 100.0f, 10.0f), m_dragPoint);
		}
	}else{
		m_dragBody = NULL;
		m_dragPoint = -1;
	}

	if (m_touchF == true){
		if (m_dragBody == NULL){
			int body;
			m_world->getClosestPointMass(Vector2(m_dragX, m_dragY), body, m_dragPoint);
			m_dragBody = m_world->getBody(body);


			//click testing
			for (size_t i = 0; i < m_gameBodies.size(); i++){
				if (m_gameBodies[i]->GetBody()->contains(Vector2(m_dragX, m_dragY))){
					if (m_gameBodies[i]->GetName() == "options_libs"){
						m_alphaScale = 1.0f;
						addStateAtTop(new JellyOptionLib(*this));
					}else if (m_gameBodies[i]->GetName() == "options_credits"){
						m_alphaScale = 1.0f;
						addStateAtTop(new JellyOptionCredit(*this));
					}else if (m_gameBodies[i]->GetName() == "options_keys"){
						m_alphaScale = 1.0f;
						addStateAtTop(new JellyOptionControl(*this));
					}else if (m_gameBodies[i]->GetName() == "options_sound"){
						m_alphaScale = 1.0f;
						MusicManager::Get().playFastEngine();
						addStateAtTop(new JellyOptionSound(*this));
					}
				}
			}
		}
	}

	if(keyboard.keyPressed(Keyboard::KEY_S)){

		if (m_menuBodies[m_menuBodySelected]->GetName() == "options_libs"){
			m_alphaScale = 1.0f;
			addStateAtTop(new JellyOptionLib(*this));
		}else if (m_menuBodies[m_menuBodySelected]->GetName() == "options_credits"){
			m_alphaScale = 1.0f;
			addStateAtTop(new JellyOptionCredit(*this));
		}else if (m_menuBodies[m_menuBodySelected]->GetName() == "options_keys"){
			m_alphaScale = 1.0f;
			addStateAtTop(new JellyOptionControl(*this));
		}else if (m_menuBodies[m_menuBodySelected]->GetName() == "options_sound"){
			m_alphaScale = 1.0f;
			MusicManager::Get().playFastEngine();
			addStateAtTop(new JellyOptionSound(*this));
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_D)) {

		auto shader = Globals::shaderManager.getAssetPointer("quad");
		shader->use();
		shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
		shader->unuse();

		State::m_isRunning = false;

		return;
	}
}

void JellyOptions::fixedUpdate() {
	if (!m_states.empty())
		m_states.top()->fixedUpdate();
}

void JellyOptions::update() {
	if (!m_states.empty()) {
		m_states.top()->update();
		if (!m_states.top()->isRunning()) {
			delete m_states.top();
			m_states.pop();
		}
	}else {
		processInput();

		//Update physic
		for (int i = 0; i < 6; i++) {
			m_world->update(0.004f);

			for (size_t i = 0; i < m_gameBodies.size(); i++)
				m_gameBodies[i]->Update(0.004f);
		}

		//update alpha scale
		m_alphaScale += (m_scaleFactor  * m_dt);

		if (m_alphaScale > 1.0f) {
			m_scaleFactor = -2.0f;
		}

		if (m_alphaScale < 0.0f) {
			m_scaleFactor = 2.0f;
		}		
	}
}

void JellyOptions::renderBackground() {
	auto shader = Globals::shaderManager.getAssetPointer("quad_back");

	shader->use();
	Globals::textureManager.get("paper").bind(0);

	for (int y = 0; y < m_rows; y++) {
		for (int x = 0; x < m_columns; x++) {

			int posx = m_backWidth * x;
			int posy = Application::Height - (m_backHeight * (1 + y));

			shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(posx), static_cast<float>(posy), 0.0f) * Matrix4f::Scale(static_cast<float>(m_backWidth), static_cast<float>(m_backHeight), 1.0f));
			Globals::shapeManager.get("quad_aligned").drawRaw();
		}
	}
	Globals::textureManager.get("paper").unbind(0);

	shader->unuse();
}

void JellyOptions::renderLevel() {
	//menu level
	for (size_t i = 0; i < m_gameBodies.size(); i++) {
		if (m_gameBodies[i]->GetBody()->getIsStatic()) {
			m_gameBodies[i]->Draw(m_jellyProjection);
		}
	}
}

void JellyOptions::renderControls() {
	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(Application::Width / 2), static_cast<float>(25), 0.0f)* Matrix4f::Scale(static_cast<float>(78 * 0.4f), static_cast<float>(78 * 0.4f), 1.0f));
	shader->loadVector("u_texRect", Vector4f(102.0f / m_controlsWidth, (m_controlsHeight - (5.0f + 78.0f)) / m_controlsHeight, (102.0f + 78.0f) / m_controlsWidth, (m_controlsHeight - 5.0f) / m_controlsHeight));
	
	Globals::textureManager.get("controls").bind(0);
	Globals::shapeManager.get("quad_half").drawRaw();
	Globals::textureManager.get("controls").unbind(0);
	shader->unuse();
}

void JellyOptions::render() {

	if (!m_states.empty()) {
		m_states.top()->render();
	}else {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderBackground();

		for (size_t i = 0; i < m_gameBodies.size(); i++) {
			m_gameBodies[i]->SetLineColor(Vector4f(0, 0, 0, 1.0f));
		}

		//set alpha value for selected menu
		m_menuBodies[m_menuBodySelected]->SetLineColor(Vector4f(0, 0, 1, m_alphaScale));

		//menu level
		for (size_t i = 0; i < m_gameBodies.size(); i++) {
			m_gameBodies[i]->Draw(m_jellyProjection);
		}

		renderControls();

		Globals::spritesheetManager.getAssetPointer("jelly_font")->bind(0);
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_64"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_64").getWidth("Options") / 2), static_cast<float>(Application::Height - 87), "Options", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_64"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_64").getWidth("Options") / 2), static_cast<float>(Application::Height - 85), "Options", Vector4f(1.0f, 0.65f, 0.0f, 1.0f));
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 30), static_cast<float>(6), "Back", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
		Fontrenderer::Get().drawBuffer();
		Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);
	}
}
/////////////////////////////////////////
JellyOptionState::JellyOptionState(JellyOptions& machine) : m_machine(machine) {
	m_orthographic.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, -1.0f, 1.0f);
}

void JellyOptionState::resize(int deltaW, int deltaH) {
	m_orthographic.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, -1.0f, 1.0f);
}

JellyOptionLib::JellyOptionLib(JellyOptions& machine) : JellyOptionState(machine) {
	m_centerX = Application::Width / 2;
	m_libsPosition = Application::Height - (Application::Height * 0.08f);
	m_libs.clear();

	initLibs();
}

void JellyOptionLib::fixedUpdate() {

}

void JellyOptionLib::update() {
	processInput();

	m_libsPosition -= (m_machine.m_dt * 40);

	bool reset = true;
	for (size_t i = 0; i < m_libs.size(); i++) {
		if (m_libs[i].StartPosition + m_libsPosition >(Application::Width * 0.13f)){
			reset = false;
		}
	}

	if (reset){
		m_libsPosition = Application::Height - (Application::Height * 0.08f);;
	}
}

void JellyOptionLib::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_machine.renderBackground();
	m_machine.renderLevel();
	m_machine.renderControls();

	Globals::spritesheetManager.getAssetPointer("jelly_font")->bind(0);
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_64"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_64").getWidth("Libs") / 2), static_cast<float>(Application::Height - 87), "Libs", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_64"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_64").getWidth("Libs") / 2), static_cast<float>(Application::Height - 85), "Libs", Vector4f(1.0f, 0.65f, 0.0f, 1.0f));

	for (size_t i = 0; i < m_libs.size(); i++){
		if (m_libs[i].StartPosition + m_libsPosition  >(Application::Width * 0.13f) && m_libs[i].StartPosition + m_libsPosition < (Application::Height - (Application::Height * 0.08f))){

			Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(m_centerX - Globals::fontManager.get("jelly_32").getWidth(m_libs[i].Content) / 2), static_cast<float>(Application::Height - (m_libs[i].StartPosition + m_libsPosition)), m_libs[i].Content, Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
			Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(m_centerX - Globals::fontManager.get("jelly_32").getWidth(m_libs[i].Content) / 2), static_cast<float>(Application::Height - (m_libs[i].StartPosition - 2 + m_libsPosition)), m_libs[i].Content, Vector4f(1.0f, 0.65f, 0.0f, 1.0f));
		}
	}

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 30), 6, "Back", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().drawBuffer();
	Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);
} 

void JellyOptionLib::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
		//m_machine.stopState();
	}
}

void JellyOptionLib::processInput() {
	Keyboard &keyboard = Keyboard::instance();

	if (keyboard.keyPressed(Keyboard::KEY_D)) {
		m_isRunning = false;
		return;
	}
}

void JellyOptionLib::resize(int deltaW, int deltaH) {
	JellyOptionState::resize(deltaW, deltaH);
	m_centerX = Application::Width / 2;
	m_libsPosition = Application::Height - (Application::Height * 0.08f);
}

void JellyOptionLib::initLibs() {
	m_libs.push_back(Text("Libs used for PC/Vita/Switch", 0));
	m_libs.push_back(Text("freetype", 82));
	m_libs.push_back(Text("freetype_gl", 142));
	m_libs.push_back(Text("glad", 202));
	m_libs.push_back(Text("glew", 262));
	m_libs.push_back(Text("glfw", 322));
	m_libs.push_back(Text("glm", 382));
	m_libs.push_back(Text("irrKlang", 442));
	m_libs.push_back(Text("JellyPhysics", 502));
	m_libs.push_back(Text("ogg", 562));
	m_libs.push_back(Text("SDL2", 622));
	m_libs.push_back(Text("stb", 682));
	m_libs.push_back(Text("speexdsp", 742));
	m_libs.push_back(Text("tinyxml", 802));
	m_libs.push_back(Text("vorbisfile", 862));
	m_libs.push_back(Text("vorbisenc", 922));
	m_libs.push_back(Text("vorbis", 9802));
}

JellyOptionCredit::JellyOptionCredit(JellyOptions& machine) : JellyOptionState(machine) {
	m_centerX = Application::Width / 2;
	m_creditsPosition = Application::Height - (Application::Height * 0.08f);
	m_credits.clear();
	initCredits();
}

void JellyOptionCredit::fixedUpdate() {}

void JellyOptionCredit::update() {
	processInput();

	m_creditsPosition -= (m_machine.m_dt * 40);

	bool reset = true;
	for (size_t i = 0; i < m_credits.size(); i++){
		if (m_credits[i].StartPosition + m_creditsPosition >(Application::Width * 0.13f)){
			reset = false;
		}
	}

	if (reset){
		m_creditsPosition = Application::Height - (Application::Height * 0.08f);;
	}
}

void JellyOptionCredit::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_machine.renderBackground();
	m_machine.renderLevel();
	m_machine.renderControls();

	Globals::spritesheetManager.getAssetPointer("jelly_font")->bind(0);
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_64"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_64").getWidth("Credits") / 2), static_cast<float>(Application::Height - 87), "Credits", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_64"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_64").getWidth("Credits") / 2), static_cast<float>(Application::Height - 85), "Credits", Vector4f(1.0f, 0.65f, 0.0f, 1.0f));

	for (size_t i = 0; i < m_credits.size(); i++){
		if (m_credits[i].StartPosition + m_creditsPosition  >(Application::Width * 0.13f) && m_credits[i].StartPosition + m_creditsPosition < (Application::Height - (Application::Height * 0.08f))){
			Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(m_centerX - Globals::fontManager.get("jelly_32").getWidth(m_credits[i].Content) / 2), static_cast<float>(Application::Height - (m_credits[i].StartPosition + m_creditsPosition)), m_credits[i].Content, Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
			Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(m_centerX - Globals::fontManager.get("jelly_32").getWidth(m_credits[i].Content) / 2), static_cast<float>(Application::Height - (m_credits[i].StartPosition - 2 + m_creditsPosition)), m_credits[i].Content, Vector4f(1.0f, 0.65f, 0.0f, 1.0f));
		}
	}

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 30), 6, "Back", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().drawBuffer();
	Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);
}

void JellyOptionCredit::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void JellyOptionCredit::processInput() {
	Keyboard &keyboard = Keyboard::instance();

	if (keyboard.keyPressed(Keyboard::KEY_D)) {
		m_isRunning = false;
		return;
	}
}

void JellyOptionCredit::resize(int deltaW, int deltaH) {
	JellyOptionState::resize(deltaW, deltaH);
	m_centerX = Application::Width / 2;
	m_creditsPosition = Application::Height - (Application::Height * 0.08f);
}

void JellyOptionCredit::initCredits() {
	m_credits.push_back(Text("Thanks", 0));
	m_credits.push_back(Text("Walaber for original game", 82));
	m_credits.push_back(Text("Shadow for car skins", 142));
	m_credits.push_back(Text("Ruben Wolfe, SMOKE, TheFloW", 202));
	m_credits.push_back(Text("for support and testing", 242));
	m_credits.push_back(Text("Rinnegatamante and EasyRPG Team", 302));
	m_credits.push_back(Text("for the Audio Decoder used for Vita sound module", 342));
	m_credits.push_back(Text("St3f  for splash screen", 402));
	m_credits.push_back(Text("Team Molecule for HENkaku", 462));
	m_credits.push_back(Text("Everybody who contributed to vitasdk", 522));
	m_credits.push_back(Text("Switchbrew team", 582));
	m_credits.push_back(Text("Everybody who contributed to libnx", 642));
}

JellyOptionControl::JellyOptionControl(JellyOptions& machine) : JellyOptionState(machine) {
	m_carActions.clear();
	m_actionTranslation.clear();
	initActionNames();

	m_carKeyboardMapping.insert(std::pair<CarAction, Keyboard::Key>(CarAction::Left, Keyboard::Key::KEY_LEFT));
	m_carKeyboardMapping.insert(std::pair<CarAction, Keyboard::Key>(CarAction::Right, Keyboard::Key::KEY_RIGHT));

	m_carKeyboardMapping.insert(std::pair<CarAction, Keyboard::Key>(CarAction::Transform, Keyboard::Key::KEY_S));
	m_carKeyboardMapping.insert(std::pair<CarAction, Keyboard::Key>(CarAction::Ballon, Keyboard::Key::KEY_D));
	m_carKeyboardMapping.insert(std::pair<CarAction, Keyboard::Key>(CarAction::Tire, Keyboard::Key::KEY_A));
	m_carKeyboardMapping.insert(std::pair<CarAction, Keyboard::Key>(CarAction::Map, Keyboard::Key::KEY_W));

	m_carKeyboardMapping.insert(std::pair<CarAction, Keyboard::Key>(CarAction::RotateLeft, Keyboard::Key::KEY_Q));
	m_carKeyboardMapping.insert(std::pair<CarAction, Keyboard::Key>(CarAction::RotateRight, Keyboard::Key::KEY_E));

	loadSettings("JellyKeySettings.xml");

	m_centerX = Application::Width / 2;
	m_alphaScale = 1.0f;
	m_scaleFactor = 0.01f;

	m_changeBinding = false;
	m_selctedPosition = 0;

	m_controlsWidth = static_cast<float>(Globals::textureManager.get("controls").getWidth());
	m_controlsHeight = static_cast<float>(Globals::textureManager.get("controls").getHeight());
}

JellyOptionControl::~JellyOptionControl() {
	saveSettings("JellyKeySettings.xml");
}

void JellyOptionControl::fixedUpdate() {}

void JellyOptionControl::update() {
	processInput();

	m_alphaScale += (m_scaleFactor  * m_machine.m_dt);

	if (m_alphaScale > 1.0f){
		m_scaleFactor = -2.0f;
	}

	if (m_alphaScale < 0.0f){
		m_scaleFactor = 2.0f;
	}
}

void JellyOptionControl::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_machine.renderBackground();
	m_machine.renderLevel();
	m_machine.renderControls();


	int startPosition = Application::Height / 2 - 100;
	int step = 45;

	const Texture& back = Globals::textureManager.get("bar_blue");
	Vector3f pos = m_selctedPosition < 4 ? Vector3f(m_centerX - 145, startPosition - 20 + (m_selctedPosition * step), 0.0f) : Vector3f(m_centerX + 145, startPosition - 20 + (m_selctedPosition * step) - (step * 4), 0.0f);

	
	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(pos) * Matrix4f::Scale(static_cast<float>(back.getWidth()) * 0.53f, static_cast<float>(back.getHeight()) * 1.1f, 1.0f));
	shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, m_changeBinding ? m_alphaScale : 1.0f));

	back.bind(0);
	Globals::shapeManager.get("quad_half").drawRaw();
	back.unbind();

	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	
	Globals::textureManager.get("controls").bind(0);

	int startY = startPosition + 85;
	//R-Trigger
	if (!(m_changeBinding && m_selctedPosition == 3)){
		shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(static_cast<float>(m_centerX - 50), static_cast<float>(Application::Height - startY), 0.0f) * Matrix4f::Scale(128.0f * 0.5f, 60.0f * 0.5f, 1.0f));
		shader->loadVector("u_texRect", Vector4f(369.0f / m_controlsWidth, (m_controlsHeight - (11.0f + 60.0f)) / m_controlsHeight, (369.0f + 128.0f) / m_controlsWidth, (m_controlsHeight - 11.0f) / m_controlsHeight));

		Globals::shapeManager.get("quad_half").drawRaw();
	}

	startY += step;
	//L-Triggert
	if (!(m_changeBinding && m_selctedPosition == 2)) {
		shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(static_cast<float>(m_centerX - 50), static_cast<float>(Application::Height - startY), 0.0f) * Matrix4f::Scale(128.0f* 0.5f, 60.0f * 0.5f, 1.0f));
		shader->loadVector("u_texRect", Vector4f(6.0f / m_controlsWidth, (m_controlsHeight - (87.0f + 60.0f)) / m_controlsHeight, (6.0f + 128.0f) / m_controlsWidth, (m_controlsHeight - 87.0f) / m_controlsHeight));

		Globals::shapeManager.get("quad_half").drawRaw();
	}

	startY += step;
	//Right
	if (!(m_changeBinding && m_selctedPosition == 1)) {
		shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(static_cast<float>(m_centerX - 50), static_cast<float>(Application::Height - startY), 0.0f) * Matrix4f::Scale(88.0f * 0.4f, 68.0f * 0.4f, 1.0f));
		shader->loadVector("u_texRect", Vector4f(413.0f / m_controlsWidth, (m_controlsHeight - (82.0f + 68.0f)) / m_controlsHeight, (413.0f + 88.0f) / m_controlsWidth, (m_controlsHeight - 82.0f) / m_controlsHeight));

		Globals::shapeManager.get("quad_half").drawRaw();
	}

	startY += step;
	//Left
	if (!(m_changeBinding && m_selctedPosition == 0)) {
		shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(static_cast<float>(m_centerX - 50), static_cast<float>(Application::Height - startY), 0.0f) * Matrix4f::Scale(88.0f * 0.4f, 68.0f * 0.4f, 1.0f));
		shader->loadVector("u_texRect", Vector4f(313.0f / m_controlsWidth, (m_controlsHeight - (88.0f + 68.0f)) / m_controlsHeight, (313.0f + 88.0f) / m_controlsWidth, (m_controlsHeight - 88.0f) / m_controlsHeight));

		Globals::shapeManager.get("quad_half").drawRaw();
	}
	
	startY = startPosition + 85;
	//Triangle
	if (!(m_changeBinding && m_selctedPosition == 7)) {
		shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(static_cast<float>(m_centerX + 50), static_cast<float>(Application::Height - startY), 0.0f) * Matrix4f::Scale(78.0f * 0.4f, 78.0f * 0.4f, 1.0f));
		shader->loadVector("u_texRect", Vector4f(17.0f / m_controlsWidth, (m_controlsHeight - (5.0f + 78.0f)) / m_controlsHeight, (17.0f + 78.0f) / m_controlsWidth, (m_controlsHeight - 5.0f) / m_controlsHeight));

		Globals::shapeManager.get("quad_half").drawRaw();
	}

	startY += step;
	//Square
	if (!(m_changeBinding && m_selctedPosition == 6)) {
		shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(static_cast<float>(m_centerX + 50), static_cast<float>(Application::Height - startY), 0.0f) * Matrix4f::Scale(78.0f * 0.4f, 78.0f * 0.4f, 1.0f));
		shader->loadVector("u_texRect", Vector4f(273.0f / m_controlsWidth, (m_controlsHeight - (5.0f + 78.0f)) / m_controlsHeight, (273.0f + 78.0f) / m_controlsWidth, (m_controlsHeight - 5.0f) / m_controlsHeight));

		Globals::shapeManager.get("quad_half").drawRaw();
	}
	startY += step;
	//Circle
	if (!(m_changeBinding && m_selctedPosition == 5)) {
		shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(static_cast<float>(m_centerX + 50), static_cast<float>(Application::Height - startY), 0.0f) * Matrix4f::Scale(78.0f * 0.4f, 78.0f * 0.4f, 1.0f));
		shader->loadVector("u_texRect", Vector4f(102.0f / m_controlsWidth, (m_controlsHeight - (5.0f + 78.0f)) / m_controlsHeight, (102.0f + 78.0f) / m_controlsWidth, (m_controlsHeight - 5.0f) / m_controlsHeight));

		Globals::shapeManager.get("quad_half").drawRaw();
	}
	startY += step;
	//Cross
	if (!(m_changeBinding && m_selctedPosition == 4)) {
		shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(static_cast<float>(m_centerX + 50), static_cast<float>(Application::Height - startY), 0.0f) * Matrix4f::Scale(78.0f * 0.4f, 78.0f * 0.4f, 1.0f));
		shader->loadVector("u_texRect", Vector4f(186.0f / m_controlsWidth, (m_controlsHeight - (5.0f + 78.0f)) / m_controlsHeight, (186.0f + 78.0f) / m_controlsWidth, (m_controlsHeight - 5.0f) / m_controlsHeight));

		Globals::shapeManager.get("quad_half").drawRaw();
	}

	Globals::textureManager.get("controls").unbind(0);
	shader->unuse();

	
	Globals::spritesheetManager.getAssetPointer("jelly_font")->bind(0);
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_64"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_64").getWidth("Car Controls") / 2), static_cast<float>(Application::Height - 87), "Car Controls", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_64"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_64").getWidth("Car Controls") / 2), static_cast<float>(Application::Height - 85), "Car Controls", Vector4f(1.0f, 0.65f, 0.0f, 1.0f));

	startY = startPosition;

	//4 left positions
	for (size_t i = 0; i < 4; i++){
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(m_centerX - 100 - Globals::fontManager.get("jelly_32").getWidth(m_actionTranslation[m_carActions[i]])), static_cast<float>(Application::Height - startY), m_actionTranslation[m_carActions[i]], Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(m_centerX - 100 - Globals::fontManager.get("jelly_32").getWidth(m_actionTranslation[m_carActions[i]])), static_cast<float>(Application::Height - (startY - 2)), m_actionTranslation[m_carActions[i]], Vector4f(1.0f, 0.65f, 0.0f, 1.0f));
		startY += step;
	}

	//4 right positions
	startY = startPosition;
	for (size_t i = 4; i < m_carActions.size(); i++){
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(m_centerX + 100), static_cast<float>(Application::Height - startY), m_actionTranslation[m_carActions[i]], Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(m_centerX + 100), static_cast<float>(Application::Height - (startY - 2)), m_actionTranslation[m_carActions[i]], Vector4f(1.0f, 0.65f, 0.0f, 1.0f));
		startY += step;
	}

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 30), 6, "Back", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));

	Fontrenderer::Get().drawBuffer();
	Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);
}

void JellyOptionControl::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {

		auto shader = Globals::shaderManager.getAssetPointer("quad");
		shader->use();
		shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
		shader->unuse();

		m_isRunning = false;
	}
}

void JellyOptionControl::processInput() {
	Keyboard &keyboard = Keyboard::instance();

	/*if (_changeBinding){
		//if (_inputHelper->ChangeInputForAction(_selectedAction)){
			_changeBinding = false;
		//}
	}else{*/
		//move up
		if(keyboard.keyPressed(Keyboard::KEY_UP)){
			m_selctedPosition--;
			if (m_selctedPosition < 0){
				m_selctedPosition = 0;
			}
		}

		//move down
		if (keyboard.keyPressed(Keyboard::KEY_DOWN)){
			m_selctedPosition++;
			if (m_selctedPosition > m_carActions.size() - 1){
				m_selctedPosition = m_carActions.size() - 1;
			}
		}

		if (keyboard.keyPressed(Keyboard::KEY_ENTER) && !keyboard.keyDown(Keyboard::KEY_RALT)){
			//check action to process
			m_selectedAction = m_carActions[m_selctedPosition];
			m_changeBinding = !m_changeBinding;

			//_inputHelper->UpdateAllInputs(true);
		}

		if (keyboard.keyPressed(Keyboard::KEY_D)) {
			m_isRunning = false;
			return;
		}
	//}
}

void JellyOptionControl::resize(int deltaW, int deltaH) {
	JellyOptionState::resize(deltaW, deltaH);
	m_centerX = Application::Width / 2;
}

void JellyOptionControl::initActionNames() {
	m_carActions.push_back(CarAction::Left);
	m_carActions.push_back(CarAction::Right);
	m_carActions.push_back(CarAction::RotateLeft);
	m_carActions.push_back(CarAction::RotateRight);

	m_carActions.push_back(CarAction::Transform);
	m_carActions.push_back(CarAction::Ballon);
	m_carActions.push_back(CarAction::Tire);
	m_carActions.push_back(CarAction::Map);

	m_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::Left, "Left"));
	m_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::Right, "Right"));
	m_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::RotateLeft, "Rotate Left"));
	m_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::RotateRight, "Rotate Right"));

	m_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::Transform, "Transform"));
	m_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::Ballon, "Ballon"));
	m_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::Tire, "Sticky tire"));
	m_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::Map, "Map"));
}

void JellyOptionControl::loadSettings(std::string path) {
	std::ifstream is(path, std::ifstream::in);

	if (!is.is_open())
		return;

	is.seekg(0, is.end);
	std::streamoff length = is.tellg();
	is.seekg(0, is.beg);

	unsigned char* buffer = new unsigned char[length];
	is.read(reinterpret_cast<char*>(buffer), length);
	is.close();

	TiXmlDocument doc;
	if (!doc.LoadContent(buffer, static_cast<int>(length))) {
		return;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	std::map<int, int> actionKeyMapping;

	TiXmlElement* ObjectNode = pElem = hDoc.FirstChild("Settings").FirstChild().Element();
	for (ObjectNode; ObjectNode; ObjectNode = ObjectNode->NextSiblingElement()){
		int action = atof(ObjectNode->Attribute("action"));
		int key = atof(ObjectNode->Attribute("key"));

		actionKeyMapping.insert(std::pair<int, int>(action, key));
	}

	if (actionKeyMapping.size() > 0){
		for (std::map<int, int>::iterator iterator = actionKeyMapping.begin(); iterator != actionKeyMapping.end(); iterator++){
			CarAction action = static_cast<CarAction>(iterator->first);

			Keyboard::Key code = static_cast<Keyboard::Key>(iterator->second);
			m_carKeyboardMapping.insert(std::pair<CarAction, Keyboard::Key>(action, code));
			
		}
	}
}

void JellyOptionControl::saveSettings(std::string path) {
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);

	//root
	TiXmlElement * root = new TiXmlElement("Settings");
	doc.LinkEndChild(root);

	//get mapping
	std::map<int, int> actionKeyMapping = getActionKeyMapping();

	typedef std::map<int, int>::iterator it_type;
	for (it_type iterator = actionKeyMapping.begin(); iterator != actionKeyMapping.end(); iterator++){
		TiXmlElement * cxn = new TiXmlElement("ActionMapping");
		root->LinkEndChild(cxn);
		cxn->SetDoubleAttribute("action", iterator->first);
		cxn->SetDoubleAttribute("key", iterator->second);
	}

	doc.SaveFile(path.c_str());
}

std::map<int, int> JellyOptionControl::getActionKeyMapping(){
	std::map<int, int> _actionKeyMapping;

	for (int i = 0; i < CarAction::Count; i++){
		CarAction action = static_cast<CarAction>(i);	
		_actionKeyMapping.insert(std::pair<int, int>(i, static_cast<int>(m_carKeyboardMapping[action])));
	}
	return _actionKeyMapping;
}

JellyOptionSound::JellyOptionSound(JellyOptions& machine) : JellyOptionState(machine), 
carVolume(SceneManager::Get().getScene("scene").m_soundSettings.carVolume),
soundsVolume(SceneManager::Get().getScene("scene").m_soundSettings.soundsVolume),
musicVolume(SceneManager::Get().getScene("scene").m_soundSettings.musicVolume) {
	
	m_soundPosition = 0;
	m_alphaScale = 1.0f;
	m_scaleFactor = 0.01f;

	m_optionsCarLevel = (int)round(carVolume * 10.0f);
	m_optionsSoundLevel = (int)round(soundsVolume * 10.0f);
	m_optionsMusicLevel = (int)round(musicVolume * 10.0f);
} 

JellyOptionSound::~JellyOptionSound() {
	SceneManager::Get().getScene("scene").saveSettings("JellyAudioSettings.xml");
}

void JellyOptionSound::fixedUpdate() {}

void JellyOptionSound::update() {
	processInput();

	m_alphaScale += (m_scaleFactor  * m_machine.m_dt);

	if (m_alphaScale > 1.0f){
		m_scaleFactor = -2.0f;
	}

	if (m_alphaScale < 0.0f){
		m_scaleFactor = 2.0f;
	}
}

void JellyOptionSound::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_machine.renderBackground();
	m_machine.renderLevel();
	m_machine.renderControls();

	int leftSpritePosX = Application::Width / 2 - (260);
	int rightSpritePosX = Application::Width / 2 + (260);
	int leftRightPosY = Application::Height / 2 + (Application::Height * 0.08f);

	const Texture* texture = &Globals::textureManager.get("select_left");
	texture->bind(0);

	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	Vector3f pos = Vector3f(leftSpritePosX, leftRightPosY - 140 - 5, 0.0f);
	shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(pos) * Matrix4f::Scale(static_cast<float>(texture->getWidth()) * 0.8f, static_cast<float>(texture->getHeight()) * 0.8f, 1.0f));
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, m_soundPosition == 0 ? m_alphaScale : 1.0f));
	shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	Globals::shapeManager.get("quad_half").drawRaw();

	pos = Vector3f(leftSpritePosX, leftRightPosY - 5, 0.0f);
	shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(pos) * Matrix4f::Scale(static_cast<float>(texture->getWidth()) * 0.8f, static_cast<float>(texture->getHeight()) * 0.8f, 1.0f));
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, m_soundPosition == 1 ? m_alphaScale : 1.0f));
	Globals::shapeManager.get("quad_half").drawRaw();

	pos = Vector3f(leftSpritePosX, leftRightPosY + 140 - 5, 0.0f);
	shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(pos) * Matrix4f::Scale(static_cast<float>(texture->getWidth()) * 0.8f, static_cast<float>(texture->getHeight()) * 0.8f, 1.0f));
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, m_soundPosition == 2 ? m_alphaScale : 1.0f));
	Globals::shapeManager.get("quad_half").drawRaw();

	texture = &Globals::textureManager.get("select_right");
	texture->bind(0);

	pos = Vector3f(rightSpritePosX, leftRightPosY - 140 - 5, 0.0f);
	shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(pos) * Matrix4f::Scale(static_cast<float>(texture->getWidth()) * 0.8f, static_cast<float>(texture->getHeight()) * 0.8f, 1.0f));
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, m_soundPosition == 0 ? m_alphaScale : 1.0f));
	Globals::shapeManager.get("quad_half").drawRaw();

	pos = Vector3f(rightSpritePosX, leftRightPosY - 5, 0.0f);
	shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(pos) * Matrix4f::Scale(static_cast<float>(texture->getWidth()) * 0.8f, static_cast<float>(texture->getHeight()) * 0.8f, 1.0f));
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, m_soundPosition == 1 ? m_alphaScale : 1.0f));
	Globals::shapeManager.get("quad_half").drawRaw();

	pos = Vector3f(rightSpritePosX, leftRightPosY + 140 - 5, 0.0f);
	shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(pos) * Matrix4f::Scale(static_cast<float>(texture->getWidth()) * 0.8f, static_cast<float>(texture->getHeight()) * 0.8f, 1.0f));
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, m_soundPosition == 2 ? m_alphaScale : 1.0f));
	Globals::shapeManager.get("quad_half").drawRaw();

	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	
	int startBar = Application::Width / 2 - (180);
	for (int i = 0; i < 10; i++) {
		if (i < m_optionsCarLevel) {
			texture = &Globals::textureManager.get("options_bar");
			texture->bind(0);

			pos = Vector3f(startBar + (i * 40), leftRightPosY - 140, 0.0f);
			shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(pos) * Matrix4f::Scale(static_cast<float>(texture->getWidth()) * 0.8f, static_cast<float>(texture->getHeight()) * 0.8f, 1.0f));
			Globals::shapeManager.get("quad_half").drawRaw();
		}else {
			texture = &Globals::textureManager.get("options_bar_blue");
			texture->bind(0);

			pos = Vector3f(startBar + (i * 40), leftRightPosY - 140, 0.0f);
			shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(pos) * Matrix4f::Scale(static_cast<float>(texture->getWidth()) * 0.8f, static_cast<float>(texture->getHeight()) * 0.8f, 1.0f));
			Globals::shapeManager.get("quad_half").drawRaw();
		}
	}

	for (int i = 0; i < 10; i++) {
		if (i < m_optionsSoundLevel) {
			texture = &Globals::textureManager.get("options_bar");
			texture->bind(0);

			pos = Vector3f(startBar + (i * 40), leftRightPosY, 0.0f);
			shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(pos) * Matrix4f::Scale(static_cast<float>(texture->getWidth()) * 0.8f, static_cast<float>(texture->getHeight()) * 0.8f, 1.0f));
			Globals::shapeManager.get("quad_half").drawRaw();
		}else {
			texture = &Globals::textureManager.get("options_bar_blue");
			texture->bind(0);

			pos = Vector3f(startBar + (i * 40), leftRightPosY, 0.0f);
			shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(pos) * Matrix4f::Scale(static_cast<float>(texture->getWidth()) * 0.8f, static_cast<float>(texture->getHeight()) * 0.8f, 1.0f));
			Globals::shapeManager.get("quad_half").drawRaw();
		}
	}

	for (int i = 0; i < 10; i++) {
		if (i < m_optionsMusicLevel) {
			texture = &Globals::textureManager.get("options_bar");
			texture->bind(0);

			pos = Vector3f(startBar + (i * 40), leftRightPosY + 140, 0.0f);
			shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(pos) * Matrix4f::Scale(static_cast<float>(texture->getWidth()) * 0.8f, static_cast<float>(texture->getHeight()) * 0.8f, 1.0f));
			Globals::shapeManager.get("quad_half").drawRaw();
		}else {
			texture = &Globals::textureManager.get("options_bar_blue");
			texture->bind(0);

			pos = Vector3f(startBar + (i * 40), leftRightPosY + 140, 0.0f);
			shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(pos) * Matrix4f::Scale(static_cast<float>(texture->getWidth()) * 0.8f, static_cast<float>(texture->getHeight()) * 0.8f, 1.0f));
			Globals::shapeManager.get("quad_half").drawRaw();
		}
	}
	
	shader->unuse();


	Globals::spritesheetManager.getAssetPointer("jelly_font")->bind(0);
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_64"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_64").getWidth("Sound Levels") / 2), static_cast<float>(Application::Height - 87), "Sound Levels", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_64"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_64").getWidth("Sound Levels") / 2), static_cast<float>(Application::Height - 85), "Sound Levels", Vector4f(1.0f, 0.65f, 0.0f, 1.0f));

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(leftSpritePosX + 30), static_cast<float>(Application::Height - (leftRightPosY + 2 - 140 - 55)), "Car", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(leftSpritePosX + 30), static_cast<float>(Application::Height - (leftRightPosY - 140 - 55)), "Car", Vector4f(1.0f, 0.65f, 0.0f, 1.0f));

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(leftSpritePosX + 30), static_cast<float>(Application::Height - (leftRightPosY + 2 - 55)), "Sounds", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(leftSpritePosX + 30), static_cast<float>(Application::Height - (leftRightPosY - 55)), "Sounds", Vector4f(1.0f, 0.65f, 0.0f, 1.0f));

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(leftSpritePosX + 30), static_cast<float>(Application::Height - (leftRightPosY + 2 + 140 - 55)), "Music", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(leftSpritePosX + 30), static_cast<float>(Application::Height - (leftRightPosY + 140 - 55)), "Music", Vector4f(1.0f, 0.65f, 0.0f, 1.0f));


	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 30), static_cast<float>(6), "Back", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().drawBuffer();
	Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);
}

void JellyOptionSound::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		auto shader = Globals::shaderManager.getAssetPointer("quad");
		shader->use();
		shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		shader->unuse();

		m_isRunning = false;
	}
}

void JellyOptionSound::processInput() {
	Keyboard &keyboard = Keyboard::instance();

	//move up
	if (keyboard.keyPressed(Keyboard::KEY_UP)){
		m_soundPosition--;
		if (m_soundPosition < 0){
			m_soundPosition = 0;
		}
		MusicManager::Get().stopEngineSound();

		if (m_soundPosition == 0){
			MusicManager::Get().playFastEngine();
		}
	}

	//move down
	if (keyboard.keyPressed(Keyboard::KEY_DOWN)){
		m_soundPosition++;
		if (m_soundPosition > 2){
			m_soundPosition = 2;
		}

		MusicManager::Get().stopEngineSound();

		if (m_soundPosition == 0){
			MusicManager::Get().playFastEngine();
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_LEFT)){
		if (m_soundPosition == 0){
			m_optionsCarLevel--;
			if (m_optionsCarLevel < 0){
				m_optionsCarLevel = 0;
			}
			carVolume = (float)m_optionsCarLevel / 10.0f;
			MusicManager::Get().setVolume(MusicManager::AudioHelperSoundEnum::Car, carVolume);
		}

		if (m_soundPosition == 1){
			m_optionsSoundLevel--;
			if (m_optionsSoundLevel < 0){
				m_optionsSoundLevel = 0;
			}
			soundsVolume = (float)m_optionsSoundLevel / 10.0f;
			MusicManager::Get().setVolume(MusicManager::AudioHelperSoundEnum::Sounds, soundsVolume);
			
			MusicManager::Get().playHitSound();
		}

		if (m_soundPosition == 2){
			m_optionsMusicLevel--;
			if (m_optionsMusicLevel < 0){
				m_optionsMusicLevel = 0;
			}
			musicVolume = (float)m_optionsMusicLevel / 10.0f;
			MusicManager::Get().setVolume(MusicManager::AudioHelperSoundEnum::Music, musicVolume);
		}
	}

	//move down
	if (keyboard.keyPressed(Keyboard::KEY_RIGHT)){
		if (m_soundPosition == 0){
			m_optionsCarLevel++;
			if (m_optionsCarLevel > 10){
				m_optionsCarLevel = 10;
			}

			carVolume = (float)m_optionsCarLevel / 10.0f;
			MusicManager::Get().setVolume(MusicManager::AudioHelperSoundEnum::Car, carVolume);
		}

		if (m_soundPosition == 1){
			m_optionsSoundLevel++;
			if (m_optionsSoundLevel > 10){
				m_optionsSoundLevel = 10;
			}

			soundsVolume = (float)m_optionsSoundLevel / 10.0f;
			MusicManager::Get().setVolume(MusicManager::AudioHelperSoundEnum::Sounds, soundsVolume);
			MusicManager::Get().playHitSound();
		}

		if (m_soundPosition == 2){
			m_optionsMusicLevel++;
			if (m_optionsMusicLevel > 10){
				m_optionsMusicLevel = 10;
			}

			musicVolume = (float)m_optionsMusicLevel / 10.0f;
			MusicManager::Get().setVolume(MusicManager::AudioHelperSoundEnum::Music, musicVolume);
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_D)) {
		MusicManager::Get().stopEngineSound();
		m_isRunning = false;
		return;
	}
}