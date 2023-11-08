#include <engine/Fontrenderer.h>
#include <tinyxml.h>

#include "JellyOptions.h"
#include "JellyMenu.h"
#include "Application.h"
#include "Globals.h"
#include "JellyHelper.h"
#include "TileSet.h"

JellyOptions::JellyOptions(StateMachine& machine) : State(machine, States::JELLYOPTIONS) {

	EventDispatcher::AddKeyboardListener(this);

	backWidth = Globals::textureManager.get("paper").getWidth();
	backHeight = Globals::textureManager.get("paper").getHeight();

	columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(backWidth));
	rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(backHeight));

	controlsWidth = static_cast<float>(Globals::textureManager.get("controls").getWidth());
	controlsHeight = static_cast<float>(Globals::textureManager.get("controls").getHeight());

	JellyHellper::Instance()->LoadShaders();

	_world = new World();

	_levelManager = new LevelManager();
	_levelManager->SetAssetsLocation("Assets/Jelly/");
	_levelManager->InitPhysic(_world);
	_levelManager->LoadCompiledLevel(_world, "options_scene.scene", "");

	//level elements
	_gameBodies = _levelManager->GetLevelBodies();

	_screenBounds = glm::vec4(-20.0f + 0, 0 + 20.0f, -4.2f - 5, -5 + 18.2f);
	_jellyProjection = glm::ortho(-20.0f + 0, 0 + 20.0f, -4.2f - 5, -5 + 18.2f, -1.0f, 1.0f);

	TileSetManager::Get().getTileSet("options").loadTileSet({
			"Assets/Jelly/Options/credits.png",
			"Assets/Jelly/Options/libs.png",
			"Assets/Jelly/Options/keyboard.png",
			"Assets/Jelly/Options/gamepad.png",
			"Assets/Jelly/Options/secret.png",
			"Assets/Jelly/Options/volume.png" });
	

	m_options = TileSetManager::Get().getTileSet("options").getAtlas();
	//Spritesheet::Safe("Options", m_options);

	_creditsTexture = TextureManager::Instance()->LoadFromFile("Assets/Jelly/Texture/credits.png");
	_libsTexture = TextureManager::Instance()->LoadFromFile("Assets/Jelly/Texture/libs.png");
	_keyboardTexture = TextureManager::Instance()->LoadFromFile("Assets/Jelly/Texture/keyboard.png");
	_gamepadTexture = TextureManager::Instance()->LoadFromFile("Assets/Jelly/Texture/gamepad.png");
	_secretTexture = TextureManager::Instance()->LoadFromFile("Assets/Jelly/Texture/secret.png");
	_volumeTexture = TextureManager::Instance()->LoadFromFile("Assets/Jelly/Texture/volume.png");

	//texture test
	for (size_t i = 0; i < _gameBodies.size(); i++){
		if (_gameBodies[i]->GetName() == "options_libs"){
			_gameBodies[i]->SetTexture(_libsTexture);
			_gameBodies[i]->SetTextureRect(TileSetManager::Get().getTileSet("options").getTextureRects()[1]);
			_menuBodies.push_back(_gameBodies[i]);
		}

		if (_gameBodies[i]->GetName() == "options_credits"){
			_gameBodies[i]->SetTexture(_creditsTexture);
			_gameBodies[i]->SetTextureRect(TileSetManager::Get().getTileSet("options").getTextureRects()[0]);
			_menuBodies.push_back(_gameBodies[i]);
		}

		if (_gameBodies[i]->GetName() == "options_keys"){
			_gameBodies[i]->SetTexture(_gamepadTexture);
			_gameBodies[i]->SetTextureRect(TileSetManager::Get().getTileSet("options").getTextureRects()[2]);
			_menuBodies.push_back(_gameBodies[i]);
		}

		if (_gameBodies[i]->GetName() == "options_sound"){
			_gameBodies[i]->SetTexture(_volumeTexture);
			_gameBodies[i]->SetTextureRect(TileSetManager::Get().getTileSet("options").getTextureRects()[5]);
			_menuBodies.push_back(_gameBodies[i]);
		}

		if (_gameBodies[i]->GetName() == "options_secret"){
			_gameBodies[i]->SetTexture(_secretTexture);
			_gameBodies[i]->SetTextureRect(TileSetManager::Get().getTileSet("options").getTextureRects()[4]);
			_menuBodies.push_back(_gameBodies[i]);
		}
	}

	_menuBodySelected = 0;
	_alphaScale = 1.0f;

	//binding
	_changeBinding = false;

	//_optionsCarLevel = AudioHelper::Instance()->GetVolume(AudioHelper::AudioHelperSoundEnum::Car) * 10.0f;
	//_optionsSoundLevel = AudioHelper::Instance()->GetVolume(AudioHelper::AudioHelperSoundEnum::Sounds) * 10.0f;
	//_optionsMusicLevel = AudioHelper::Instance()->GetVolume(AudioHelper::AudioHelperSoundEnum::Music) * 10.0f;

	dragBody = NULL;
	_selctedPosition = 0;
}

JellyOptions::~JellyOptions() {
	EventDispatcher::RemoveKeyboardListener(this);

	if (_levelManager != 0) {
		//clear level
		_levelManager->ClearLevel(_world);

		//remove level manager
		delete _levelManager;
	}

	//remove physic world
	delete _world;
	_gameBodies.clear();
}

void JellyOptions::OnKeyDown(Event::KeyboardEvent& event) {	
	if (!m_states.empty()) {
		m_states.top()->OnKeyDown(event);
	}else {
		if (event.keyCode == VK_ESCAPE) {
			m_isRunning = false;
		}
	}
}

void JellyOptions::resize(int deltaW, int deltaH) {
	columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(backWidth));
	rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(backHeight));
	m_machine.resizeState(deltaW, deltaH, States::JELLYGAME);
}

void JellyOptions::processInput() {
	Keyboard &keyboard = Keyboard::instance();

	//keys
	if (keyboard.keyPressed(Keyboard::KEY_LEFT)){
		_menuBodySelected--;

		if (_menuBodySelected < 0){
			_menuBodySelected = 0;
		}
	}

	//move down
	if (keyboard.keyPressed(Keyboard::KEY_RIGHT)){
		_menuBodySelected++;
		if (_menuBodySelected > _menuBodies.size() - 1){
			_menuBodySelected = _menuBodies.size() - 1;
		}
	}

	//touch 
	//if (_inputHelper->GetTouchCount() > 0) {
	if(false){	
		//glm::vec2 touch = _inputHelper->TouchToScreen(_screenBounds, _inputHelper->GetTouchPosition(0));

		//dragX = touch.x;
		//dragY = touch.y;

		touchF = true;
	}else{
		//testtouch = glm::vec2(0.0f, 0.0f);
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

	if(keyboard.keyPressed(Keyboard::KEY_S)){

		if (_menuBodies[_menuBodySelected]->GetName() == "options_libs"){
			addStateAtTop(new JellyOptionLib(*this));
		}else if (_menuBodies[_menuBodySelected]->GetName() == "options_credits"){
			
			addStateAtTop(new JellyOptionCredit(*this));
		}else if (_menuBodies[_menuBodySelected]->GetName() == "options_keys"){
			_selctedPosition = 0;
			_alphaScale = 1.0f;

			addStateAtTop(new JellyOptionControl(*this));
		}else if (_menuBodies[_menuBodySelected]->GetName() == "options_sound"){
			_soundPosition = 0;
			_alphaScale = 1.0f;
			//_audioHelper->PlayFastEngine();
			addStateAtTop(new JellyOptionSound(*this));
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_D)) {

		auto shader = Globals::shaderManager.getAssetPointer("quad");
		shader->use();
		shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
		shader->unuse();

		m_isRunning = false;

		return;
	}
}




JellyOptionState* JellyOptions::addStateAtTop(JellyOptionState* state) {
	if (!m_states.empty())
		m_states.top()->m_isActive = false;

	m_states.push(state);
	state->m_isActive = true;
	return state;
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
			_world->update(0.004f);

			for (size_t i = 0; i < _gameBodies.size(); i++)
				_gameBodies[i]->Update(0.004f);
		}

		//update alpha scale
		_alphaScale += (_scaleFactor  * m_dt);

		if (_alphaScale > 1.0f) {
				_scaleFactor = -2.0f;
		}

		if (_alphaScale < 0.0f) {
				_scaleFactor = 2.0f;
		}		
	}
}

void JellyOptions::renderBackground() {
	auto shader = Globals::shaderManager.getAssetPointer("quad_back");

	shader->use();
	Globals::textureManager.get("paper").bind(0);

	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < columns; x++) {

			int posx = (backWidth * x);
			int posy = Application::Height - (backHeight * (1 + y));

			shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(posx), static_cast<float>(posy), 0.0f) * Matrix4f::Scale(static_cast<float>(backWidth), static_cast<float>(backHeight), 1.0f));
			Globals::shapeManager.get("quad_aligned").drawRaw();
		}
	}
	Globals::textureManager.get("paper").unbind(0);

	shader->unuse();
}

void JellyOptions::renderLevel() {
	//menu level
	for (size_t i = 0; i < _gameBodies.size(); i++) {
		if (_gameBodies[i]->GetBody()->getIsStatic()) {
			_gameBodies[i]->Draw(_jellyProjection);
		}
	}
}

void JellyOptions::renderControls() {
	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();

	Globals::textureManager.get("controls").bind(0);


	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(Application::Width / 2), static_cast<float>(25), 0.0f)* Matrix4f::Scale(static_cast<float>(78 * 0.4f), static_cast<float>(78 * 0.4f), 1.0f));
	shader->loadVector("u_texRect", Vector4f(102.0f / controlsWidth, (controlsHeight - (5.0f + 78.0f)) / controlsHeight, (102.0f + 78.0f) / controlsWidth, (controlsHeight - 5.0f) / controlsHeight));

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

		for (size_t i = 0; i < _gameBodies.size(); i++) {
			_gameBodies[i]->SetLineColor(glm::vec4(0, 0, 0, 1.0f));
		}

		//set alpha value for selected menu
		_menuBodies[_menuBodySelected]->SetLineColor(glm::vec4(0, 0, 1, _alphaScale));

		//menu level
		for (size_t i = 0; i < _gameBodies.size(); i++) {
			_gameBodies[i]->Draw(_jellyProjection);
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

}

JellyOptionLib::JellyOptionLib(JellyOptions& machine) : JellyOptionState(machine) {
	centerX = Application::Width / 2;
	_libsPosition = Application::Height - (Application::Height * 0.08f);
	_libs.clear();

	initLibs();
}

void JellyOptionLib::fixedUpdate() {

}

void JellyOptionLib::update() {
	processInput();

	_libsPosition -= (m_machine.m_dt * 40);

	bool reset = true;
	for (size_t i = 0; i < _libs.size(); i++) {
		if (_libs[i].StartPosition + _libsPosition >(Application::Width * 0.13f)){
			reset = false;
		}
	}

	if (reset){
		_libsPosition = Application::Height - (Application::Height * 0.08f);;
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

	for (size_t i = 0; i < _libs.size(); i++){
		if (_libs[i].StartPosition + _libsPosition  >(Application::Width * 0.13f) && _libs[i].StartPosition + _libsPosition < (Application::Height - (Application::Height * 0.08f))){

			Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(centerX - Globals::fontManager.get("jelly_32").getWidth(_libs[i].Content) / 2), static_cast<float>(Application::Height - (_libs[i].StartPosition + _libsPosition)), _libs[i].Content, Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
			Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(centerX - Globals::fontManager.get("jelly_32").getWidth(_libs[i].Content) / 2), static_cast<float>(Application::Height - (_libs[i].StartPosition - 2 + _libsPosition)), _libs[i].Content, Vector4f(1.0f, 0.65f, 0.0f, 1.0f));
		}
	}

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 30), 6, "Back", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().drawBuffer();
	Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);
} 

void JellyOptionLib::resize(int deltaW, int deltaH) {
	m_machine.resize(deltaW, deltaH);
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

void JellyOptionLib::initLibs() {
	_libs.push_back(Text("Libs used for PC/Vita/Switch", 0));
	_libs.push_back(Text("freetype", 82));
	_libs.push_back(Text("freetype_gl", 142));
	_libs.push_back(Text("glad", 202));
	_libs.push_back(Text("glew", 262));
	_libs.push_back(Text("glfw", 322));
	_libs.push_back(Text("glm", 382));
	_libs.push_back(Text("irrKlang", 442));
	_libs.push_back(Text("JellyPhysics", 502));
	_libs.push_back(Text("ogg", 562));
	_libs.push_back(Text("SDL2", 622));
	_libs.push_back(Text("stb", 682));
	_libs.push_back(Text("speexdsp", 742));
	_libs.push_back(Text("tinyxml", 802));
	_libs.push_back(Text("vorbisfile", 862));
	_libs.push_back(Text("vorbisenc", 922));
	_libs.push_back(Text("vorbis", 9802));
}

JellyOptionCredit::JellyOptionCredit(JellyOptions& machine) : JellyOptionState(machine) {
	centerX = Application::Width / 2;
	_creditsPosition = Application::Height - (Application::Height * 0.08f);
	_credits.clear();
	initCredits();
}

void JellyOptionCredit::fixedUpdate() {}

void JellyOptionCredit::update() {
	processInput();

	_creditsPosition -= (m_machine.m_dt * 40);

	bool reset = true;
	for (size_t i = 0; i < _credits.size(); i++){
		if (_credits[i].StartPosition + _creditsPosition >(Application::Width * 0.13f)){
			reset = false;
		}
	}

	if (reset){
		_creditsPosition = Application::Height - (Application::Height * 0.08f);;
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

	for (size_t i = 0; i < _credits.size(); i++){
		if (_credits[i].StartPosition + _creditsPosition  >(Application::Width * 0.13f) && _credits[i].StartPosition + _creditsPosition < (Application::Height - (Application::Height * 0.08f))){
			Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(centerX - Globals::fontManager.get("jelly_32").getWidth(_credits[i].Content) / 2), static_cast<float>(Application::Height - (_credits[i].StartPosition + _creditsPosition)), _credits[i].Content, Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
			Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(centerX - Globals::fontManager.get("jelly_32").getWidth(_credits[i].Content) / 2), static_cast<float>(Application::Height - (_credits[i].StartPosition - 2 + _creditsPosition)), _credits[i].Content, Vector4f(1.0f, 0.65f, 0.0f, 1.0f));
		}
	}

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 30), 6, "Back", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().drawBuffer();
	Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);
}

void JellyOptionCredit::resize(int deltaW, int deltaH) {
	m_machine.resize(deltaW, deltaH);
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

void JellyOptionCredit::initCredits() {
	_credits.push_back(Text("Thanks", 0));
	_credits.push_back(Text("Walaber for original game", 82));
	_credits.push_back(Text("Shadow for car skins", 142));
	_credits.push_back(Text("Ruben Wolfe, SMOKE, TheFloW", 202));
	_credits.push_back(Text("for support and testing", 242));
	_credits.push_back(Text("Rinnegatamante and EasyRPG Team", 302));
	_credits.push_back(Text("for the Audio Decoder used for Vita sound module", 342));
	_credits.push_back(Text("St3f  for splash screen", 402));
	_credits.push_back(Text("Team Molecule for HENkaku", 462));
	_credits.push_back(Text("Everybody who contributed to vitasdk", 522));
	_credits.push_back(Text("Switchbrew team", 582));
	_credits.push_back(Text("Everybody who contributed to libnx", 642));
}


JellyOptionControl::JellyOptionControl(JellyOptions& machine) : JellyOptionState(machine) {
	_carActions.clear();
	_actionTranslation.clear();
	initActionNames();

	_carKeyboardMapping.insert(std::pair<CarAction, Keyboard::Key>(CarAction::Left, Keyboard::Key::KEY_LEFT));
	_carKeyboardMapping.insert(std::pair<CarAction, Keyboard::Key>(CarAction::Right, Keyboard::Key::KEY_RIGHT));

	_carKeyboardMapping.insert(std::pair<CarAction, Keyboard::Key>(CarAction::Transform, Keyboard::Key::KEY_S));
	_carKeyboardMapping.insert(std::pair<CarAction, Keyboard::Key>(CarAction::Ballon, Keyboard::Key::KEY_D));
	_carKeyboardMapping.insert(std::pair<CarAction, Keyboard::Key>(CarAction::Tire, Keyboard::Key::KEY_A));
	_carKeyboardMapping.insert(std::pair<CarAction, Keyboard::Key>(CarAction::Map, Keyboard::Key::KEY_W));

	_carKeyboardMapping.insert(std::pair<CarAction, Keyboard::Key>(CarAction::RotateLeft, Keyboard::Key::KEY_Q));
	_carKeyboardMapping.insert(std::pair<CarAction, Keyboard::Key>(CarAction::RotateRight, Keyboard::Key::KEY_E));

	loadSettings("JellyKeySettings.xml");

	centerX = Application::Width / 2;
	_alphaScale = 1.0f;
	_scaleFactor = 0.01f;
	_projection = glm::ortho(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, -1.0f, 1.0f);

	_changeBinding = false;
	_selctedPosition = 0;

	_backSelectSprite = new Sprite("selectBack", "Assets/Jelly/Texture/back.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");
	controlsWidth = static_cast<float>(Globals::textureManager.get("controls").getWidth());
	controlsHeight = static_cast<float>(Globals::textureManager.get("controls").getHeight());
}

JellyOptionControl::~JellyOptionControl() {
	saveSettings("JellyKeySettings.xml");
}

void JellyOptionControl::fixedUpdate() {}

void JellyOptionControl::update() {
	processInput();

	_alphaScale += (_scaleFactor  * m_machine.m_dt);

	if (_alphaScale > 1.0f){
		_scaleFactor = -2.0f;
	}

	if (_alphaScale < 0.0f){
		_scaleFactor = 2.0f;
	}
}

void JellyOptionControl::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_machine.renderBackground();
	m_machine.renderLevel();
	m_machine.renderControls();

	_backSelectSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, _changeBinding ? _alphaScale : 1.0f));
	int startPosition = Application::Height / 2 - 100;
	int step = 45;

	//draw backgroud bar
	_backSelectSprite->SetScale(glm::vec2(0.53f, 1.1f));

	if (_selctedPosition < 4){
		_backSelectSprite->SetPosition(glm::vec2(centerX - 145, startPosition - 20 + (_selctedPosition * step)));
	}else{
		_backSelectSprite->SetPosition(glm::vec2(centerX + 145, startPosition - 20 + (_selctedPosition * step) - (step * 4)));
	}

	_backSelectSprite->Draw(_projection);


	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();

	Globals::textureManager.get("controls").bind(0);

	int startY = startPosition + 85;
	//R-Trigger
	if (!(_changeBinding && _selctedPosition == 3)){
		shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(centerX - 50), static_cast<float>(startY), 0.0f) * Matrix4f::Scale(128.0f * 0.5f, 60.0f * 0.5f, 1.0f));
		shader->loadVector("u_texRect", Vector4f(369.0f / controlsWidth, (controlsHeight - (11.0f + 60.0f)) / controlsHeight, (369.0f + 128.0f) / controlsWidth, (controlsHeight - 11.0f) / controlsHeight));

		Globals::shapeManager.get("quad_half").drawRaw();
	}

	startY += step;
	//L-Triggert
	if (!(_changeBinding && _selctedPosition == 2)) {
		shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(centerX - 50), static_cast<float>(startY), 0.0f) * Matrix4f::Scale(128.0f* 0.5f, 60.0f * 0.5f, 1.0f));
		shader->loadVector("u_texRect", Vector4f(6.0f / controlsWidth, (controlsHeight - (87.0f + 60.0f)) / controlsHeight, (6.0f + 128.0f) / controlsWidth, (controlsHeight - 87.0f) / controlsHeight));

		Globals::shapeManager.get("quad_half").drawRaw();
	}

	startY += step;
	//Right
	if (!(_changeBinding && _selctedPosition == 1)) {
		shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(centerX - 50), static_cast<float>(startY), 0.0f) * Matrix4f::Scale(88.0f * 0.4f, 68.0f * 0.4f, 1.0f));
		shader->loadVector("u_texRect", Vector4f(413.0f / controlsWidth, (controlsHeight - (82.0f + 68.0f)) / controlsHeight, (413.0f + 88.0f) / controlsWidth, (controlsHeight - 82.0f) / controlsHeight));

		Globals::shapeManager.get("quad_half").drawRaw();
	}

	startY += step;
	//Left
	if (!(_changeBinding && _selctedPosition == 0)) {
		shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(centerX - 50), static_cast<float>(startY), 0.0f) * Matrix4f::Scale(88.0f * 0.4f, 68.0f * 0.4f, 1.0f));
		shader->loadVector("u_texRect", Vector4f(313.0f / controlsWidth, (controlsHeight - (88.0f + 68.0f)) / controlsHeight, (313.0f + 88.0f) / controlsWidth, (controlsHeight - 88.0f) / controlsHeight));

		Globals::shapeManager.get("quad_half").drawRaw();
	}
	
	startY = startPosition + 85;
	//Triangle
	if (!(_changeBinding && _selctedPosition == 7)) {
		shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(centerX + 50), static_cast<float>(startY), 0.0f) * Matrix4f::Scale(78.0f * 0.4f, 78.0f * 0.4f, 1.0f));
		shader->loadVector("u_texRect", Vector4f(17.0f / controlsWidth, (controlsHeight - (5.0f + 78.0f)) / controlsHeight, (17.0f + 78.0f) / controlsWidth, (controlsHeight - 5.0f) / controlsHeight));

		Globals::shapeManager.get("quad_half").drawRaw();
	}

	startY += step;
	//Square
	if (!(_changeBinding && _selctedPosition == 6)) {
		shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(centerX + 50), static_cast<float>(startY), 0.0f) * Matrix4f::Scale(78.0f * 0.4f, 78.0f * 0.4f, 1.0f));
		shader->loadVector("u_texRect", Vector4f(273.0f / controlsWidth, (controlsHeight - (5.0f + 78.0f)) / controlsHeight, (273.0f + 78.0f) / controlsWidth, (controlsHeight - 5.0f) / controlsHeight));

		Globals::shapeManager.get("quad_half").drawRaw();
	}
	startY += step;
	//Circle
	if (!(_changeBinding && _selctedPosition == 5)) {
		shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(centerX + 50), static_cast<float>(startY), 0.0f) * Matrix4f::Scale(78.0f * 0.4f, 78.0f * 0.4f, 1.0f));
		shader->loadVector("u_texRect", Vector4f(102.0f / controlsWidth, (controlsHeight - (5.0f + 78.0f)) / controlsHeight, (102.0f + 78.0f) / controlsWidth, (controlsHeight - 5.0f) / controlsHeight));

		Globals::shapeManager.get("quad_half").drawRaw();
	}
	startY += step;
	//Cross
	if (!(_changeBinding && _selctedPosition == 4)) {
		shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(centerX + 50), static_cast<float>(startY), 0.0f) * Matrix4f::Scale(78.0f * 0.4f, 78.0f * 0.4f, 1.0f));
		shader->loadVector("u_texRect", Vector4f(186.0f / controlsWidth, (controlsHeight - (5.0f + 78.0f)) / controlsHeight, (186.0f + 78.0f) / controlsWidth, (controlsHeight - 5.0f) / controlsHeight));

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
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(centerX - 100 - Globals::fontManager.get("jelly_32").getWidth(_actionTranslation[_carActions[i]])), static_cast<float>(Application::Height - startY), _actionTranslation[_carActions[i]], Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(centerX - 100 - Globals::fontManager.get("jelly_32").getWidth(_actionTranslation[_carActions[i]])), static_cast<float>(Application::Height - (startY - 2)), _actionTranslation[_carActions[i]], Vector4f(1.0f, 0.65f, 0.0f, 1.0f));
		startY += step;
	}

	//4 right positions
	startY = startPosition;
	for (size_t i = 4; i < _carActions.size(); i++){
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(centerX + 100), static_cast<float>(Application::Height - startY), _actionTranslation[_carActions[i]], Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(centerX + 100), static_cast<float>(Application::Height - (startY - 2)), _actionTranslation[_carActions[i]], Vector4f(1.0f, 0.65f, 0.0f, 1.0f));
		startY += step;
	}

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 30), 6, "Back", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));

	Fontrenderer::Get().drawBuffer();
	Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);
}

void JellyOptionControl::resize(int deltaW, int deltaH) {
	m_machine.resize(deltaW, deltaH);
}

void JellyOptionControl::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
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
			_selctedPosition--;
			if (_selctedPosition < 0){
				_selctedPosition = 0;
			}
		}

		//move down
		if (keyboard.keyPressed(Keyboard::KEY_DOWN)){
			_selctedPosition++;
			if (_selctedPosition > _carActions.size() - 1){
				_selctedPosition = _carActions.size() - 1;
			}
		}

		if (keyboard.keyPressed(Keyboard::KEY_ENTER) && !keyboard.keyDown(Keyboard::KEY_RALT)){
			//check action to process
			_selectedAction = _carActions[_selctedPosition];
			_changeBinding = !_changeBinding;

			//_inputHelper->UpdateAllInputs(true);
		}

		if (keyboard.keyPressed(Keyboard::KEY_D)) {
			m_isRunning = false;
			return;
		}
	//}
}

void JellyOptionControl::initActionNames() {
	_carActions.push_back(CarAction::Left);
	_carActions.push_back(CarAction::Right);
	_carActions.push_back(CarAction::RotateLeft);
	_carActions.push_back(CarAction::RotateRight);

	_carActions.push_back(CarAction::Transform);
	_carActions.push_back(CarAction::Ballon);
	_carActions.push_back(CarAction::Tire);
	_carActions.push_back(CarAction::Map);

	_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::Left, "Left"));
	_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::Right, "Right"));
	_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::RotateLeft, "Rotate Left"));
	_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::RotateRight, "Rotate Right"));

	_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::Transform, "Transform"));
	_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::Ballon, "Ballon"));
	_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::Tire, "Sticky tire"));
	_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::Map, "Map"));
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
			_carKeyboardMapping.insert(std::pair<CarAction, Keyboard::Key>(action, code));
			
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
		_actionKeyMapping.insert(std::pair<int, int>(i, static_cast<int>(_carKeyboardMapping[action])));
	}
	return _actionKeyMapping;
}

JellyOptionSound::JellyOptionSound(JellyOptions& machine) : JellyOptionState(machine) {
	_carVolume = 0.3f;
	_soundsVolume = 0.3f;
	_musicVolume = 0.1f;

	loadSettings("JellyAudioSettings.xml");

	_backRoundSprite = new Sprite("selectRoundBack", "Assets/Jelly/Texture/roundBack.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");
	_leftSprite = new Sprite("selectLeft", "Assets/Jelly/Texture/left.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");
	_rightSprite = new Sprite("selectRight", "Assets/Jelly/Texture/right.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");

	_barSprite = new Sprite("optionsBar", "Assets/Jelly/Texture/bar.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");
	_barBlueSprite = new Sprite("optionsBarBlue", "Assets/Jelly/Texture/barBlue.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");

	_soundPosition = 0;
	_alphaScale = 1.0f;
	_scaleFactor = 0.01f;

	_projection = glm::ortho(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, -1.0f, 1.0f);

	_optionsCarLevel = _carVolume * 10.0f;
	_optionsSoundLevel = _soundsVolume * 10.0f;
	_optionsMusicLevel = _musicVolume * 10.0f;
}

JellyOptionSound::~JellyOptionSound() {
	saveSettings("JellyAudioSettings.xml");
}

void JellyOptionSound::fixedUpdate() {}

void JellyOptionSound::update() {
	processInput();

	_alphaScale += (_scaleFactor  * m_machine.m_dt);

	if (_alphaScale > 1.0f){
		_scaleFactor = -2.0f;
	}

	if (_alphaScale < 0.0f){
		_scaleFactor = 2.0f;
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

	_leftSprite->SetScale(glm::vec2(0.8f, 0.8f));


	_leftSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, _soundPosition == 0 ? _alphaScale : 1.0f));
	_leftSprite->SetPosition(glm::vec2(leftSpritePosX, leftRightPosY - 140));
	_leftSprite->Draw(_projection);

	_leftSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, _soundPosition == 1 ? _alphaScale : 1.0f));
	_leftSprite->SetPosition(glm::vec2(leftSpritePosX, leftRightPosY));
	_leftSprite->Draw(_projection);


	_leftSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, _soundPosition == 2 ? _alphaScale : 1.0f));
	_leftSprite->SetPosition(glm::vec2(leftSpritePosX, leftRightPosY + 140));
	_leftSprite->Draw(_projection);

	//right side
	_rightSprite->SetScale(glm::vec2(0.8f, 0.8f));

	_rightSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, _soundPosition == 0 ? _alphaScale : 1.0f));
	_rightSprite->SetPosition(glm::vec2(rightSpritePosX, leftRightPosY - 140));
	_rightSprite->Draw(_projection);


	_rightSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, _soundPosition == 1 ? _alphaScale : 1.0f));
	_rightSprite->SetPosition(glm::vec2(rightSpritePosX, leftRightPosY));
	_rightSprite->Draw(_projection);

	_rightSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, _soundPosition == 2 ? _alphaScale : 1.0f));
	_rightSprite->SetPosition(glm::vec2(rightSpritePosX, leftRightPosY + 140));
	_rightSprite->Draw(_projection);

	//sound bars
	_barSprite->SetScale(glm::vec2(0.8f, 0.8f));
	_barBlueSprite->SetScale(glm::vec2(0.8f, 0.8f));

	int startBar = Application::Width / 2 - (180);

	//int startBar = 300;

	//car
	for (int i = 0; i < 10; i++){
		if (i < _optionsCarLevel){
			_barSprite->SetPosition(glm::vec2(startBar + (i * 40), leftRightPosY - 140));
			_barSprite->Draw(_projection);
		}else{
			_barBlueSprite->SetPosition(glm::vec2(startBar + (i * 40), leftRightPosY - 140));
			_barBlueSprite->Draw(_projection);
		}
	}

	//sounds
	for (int i = 0; i < 10; i++){
		if (i < _optionsSoundLevel){
			_barSprite->SetPosition(glm::vec2(startBar + (i * 40), leftRightPosY));
			_barSprite->Draw(_projection);
		}else{
			_barBlueSprite->SetPosition(glm::vec2(startBar + (i * 40), leftRightPosY));
			_barBlueSprite->Draw(_projection);
		}
	}

	//music
	for (int i = 0; i < 10; i++){
		if (i < _optionsMusicLevel){
			_barSprite->SetPosition(glm::vec2(startBar + (i * 40), leftRightPosY + 140));
			_barSprite->Draw(_projection);
		}else{
			_barBlueSprite->SetPosition(glm::vec2(startBar + (i * 40), leftRightPosY + 140));
			_barBlueSprite->Draw(_projection);
		}
	}

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

void JellyOptionSound::resize(int deltaW, int deltaH) {
	m_machine.resize(deltaW, deltaH);
}

void JellyOptionSound::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void JellyOptionSound::processInput() {
	Keyboard &keyboard = Keyboard::instance();

	//move up
	if (keyboard.keyPressed(Keyboard::KEY_UP)){
		_soundPosition--;
		if (_soundPosition < 0){
			_soundPosition = 0;
		}

		//_audioHelper->StopEngineSound();

		if (_soundPosition == 0){
			//_audioHelper->PlayFastEngine();
		}
	}

	//move down
	if (keyboard.keyPressed(Keyboard::KEY_DOWN)){
		_soundPosition++;
		if (_soundPosition > 2){
			_soundPosition = 2;
		}

		//_audioHelper->StopEngineSound();

		if (_soundPosition == 0){
			//_audioHelper->PlayFastEngine();
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_LEFT)){

		if (_soundPosition == 0){
			_optionsCarLevel--;
			if (_optionsCarLevel < 0){
				_optionsCarLevel = 0;
			}
			_carVolume = (float)_optionsCarLevel / 10.0f;
		}

		if (_soundPosition == 1){
			_optionsSoundLevel--;
			if (_optionsSoundLevel < 0){
				_optionsSoundLevel = 0;
			}
			_soundsVolume = (float)_optionsSoundLevel / 10.0f;
			//_audioHelper->PlayHitSound();
		}

		if (_soundPosition == 2){
			_optionsMusicLevel--;
			if (_optionsMusicLevel < 0){
				_optionsMusicLevel = 0;
			}
			_musicVolume = (float)_optionsMusicLevel / 10.0f;
		}
	}

	//move down
	if (keyboard.keyPressed(Keyboard::KEY_RIGHT)){

		if (_soundPosition == 0){
			_optionsCarLevel++;
			if (_optionsCarLevel > 10){
				_optionsCarLevel = 10;
			}

			_carVolume = (float)_optionsCarLevel / 10.0f;
		}

		if (_soundPosition == 1){
			_optionsSoundLevel++;
			if (_optionsSoundLevel > 10){
				_optionsSoundLevel = 10;
			}

			_soundsVolume = (float)_optionsSoundLevel / 10.0f;
			//_audioHelper->PlayHitSound();
		}

		if (_soundPosition == 2){
			_optionsMusicLevel++;
			if (_optionsMusicLevel > 10){
				_optionsMusicLevel = 10;
			}

			_musicVolume = (float)_optionsMusicLevel / 10.0f;
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_D)) {
		m_isRunning = false;
		return;
	}
}

void JellyOptionSound::loadSettings(std::string path) {

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

	TiXmlElement* ObjectNode = pElem = hDoc.FirstChild("Settings").FirstChild().Element();
	for (ObjectNode; ObjectNode; ObjectNode = ObjectNode->NextSiblingElement()){
		std::string soundName = ObjectNode->Attribute("name");

		if (soundName == "Car"){
			_carVolume = std::stof(ObjectNode->Attribute("volume"));
		}else if (soundName == "Sounds"){
			_soundsVolume = std::stof(ObjectNode->Attribute("volume"));
		}else if (soundName == "Music"){
			_musicVolume = std::stof(ObjectNode->Attribute("volume"));
		}
	}
}

void JellyOptionSound::saveSettings(std::string path) {
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);

	//root
	TiXmlElement * root = new TiXmlElement("Settings");
	doc.LinkEndChild(root);
	{
		TiXmlElement * cxn = new TiXmlElement("SoundLevel");
		root->LinkEndChild(cxn);
		cxn->SetAttribute("name", "Car");
		cxn->SetDoubleAttribute("volume", _carVolume);
	}

	{
		TiXmlElement * cxn = new TiXmlElement("SoundLevel");
		root->LinkEndChild(cxn);
		cxn->SetAttribute("name", "Sounds");
		cxn->SetDoubleAttribute("volume", _soundsVolume);
	}

	{
		TiXmlElement * cxn = new TiXmlElement("SoundLevel");
		root->LinkEndChild(cxn);
		cxn->SetAttribute("name", "Music");
		cxn->SetDoubleAttribute("volume", _musicVolume);
	}

	doc.SaveFile(path.c_str());
}