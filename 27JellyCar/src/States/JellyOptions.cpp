#include <engine/Fontrenderer.h>

#include "JellyOptions.h"
#include "JellyMenu.h"
#include "Application.h"
#include "Globals.h"
#include "JellyHelper.h"
#include "TileSet.h"

JellyOptions::JellyOptions(StateMachine& machine) : State(machine, CurrentState::JELLYOPTIONS) {

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

	InitCredits();
	InitLibs();
	InitActionNames();

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

	_optionsState = JellyOptionsState::Menu;

	dragBody = NULL;

}

JellyOptions::~JellyOptions() {
	EventDispatcher::RemoveKeyboardListener(this);

	if (_levelManager != 0){
		//clear level
		_levelManager->ClearLevel(_world);

		//remove level manager
		delete _levelManager;
	}

	//remove physic world
	delete _world;
	_gameBodies.clear();

	_credits.clear();
	_libs.clear();
	_actionTranslation.clear();
}

void JellyOptions::fixedUpdate() {}

void JellyOptions::update() {
	processInput();

	_dt = m_dt;

	if (_optionsState == Menu){
		//Update physic
		for (int i = 0; i < 6; i++){
			_world->update(0.004f);

			for (size_t i = 0; i < _gameBodies.size(); i++)
				_gameBodies[i]->Update(0.004f);
		}

		//update alpha scale
		_alphaScale += (_scaleFactor  * _dt);

		if (_alphaScale > 1.0f){
			_scaleFactor = -2.0f;
		}

		if (_alphaScale < 0.0f){
			_scaleFactor = 2.0f;
		}
	}
	
}

void JellyOptions::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

	

	for (size_t i = 0; i < _gameBodies.size(); i++) {
		_gameBodies[i]->Draw(_jellyProjection);
	}

	shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();

	Globals::textureManager.get("controls").bind(0);

	
	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(Application::Width / 2), static_cast<float>(25), 0.0f)* Matrix4f::Scale(static_cast<float>(78 * 0.4f), static_cast<float>(78 * 0.4f), 1.0f));
	shader->loadVector("u_texRect", Vector4f(102.0f / controlsWidth, (controlsHeight - (5.0f + 78.0f)) / controlsHeight, (102.0f + 78.0f) / controlsWidth, (controlsHeight - 5.0f) / controlsHeight));

	Globals::shapeManager.get("quad_half").drawRaw();

	Globals::textureManager.get("controls").unbind(0);
	shader->unuse();

	Globals::spritesheetManager.getAssetPointer("jelly_font")->bind(0);
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_64"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_64").getWidth("Options") / 2), Application::Height - 87, "Options", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_64"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_64").getWidth("Options") / 2), Application::Height - 85, "Options", Vector4f(1.0f, 0.65f, 0.0f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 30), 6, "Back", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().drawBuffer();
	Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);
}

void JellyOptions::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void JellyOptions::resize(int deltaW, int deltaH) {
	columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(backWidth));
	rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(backHeight));
	m_machine.resizeState(deltaW, deltaH, CurrentState::JELLYGAME);
}

void JellyOptions::processInput() {
	Keyboard &keyboard = Keyboard::instance();

	if (keyboard.keyPressed(Keyboard::KEY_D)) {

		auto shader = Globals::shaderManager.getAssetPointer("quad");
		shader->use();
		shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
		shader->unuse();

		m_isRunning = false;

		return;
	}
}

void JellyOptions::InitCredits(){
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

void JellyOptions::InitLibs(){
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

void JellyOptions::InitActionNames(){
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