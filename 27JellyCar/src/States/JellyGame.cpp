#include <engine/Fontrenderer.h>

#include "JellyGame.h"
#include "JellyMenu.h"
#include "Application.h"
#include "Globals.h"
#include "JellyHelper.h"
#include "SceneManager.h"

JellyGame::JellyGame(StateMachine& machine, std::string scene) : State(machine, CurrentState::JELLYGAME) {
	EventDispatcher::AddKeyboardListener(this);

	m_scene = scene;

	//std::cout << "Scene Info: " << m_scene << "  " << SceneManager::Get().getSceneInfo(m_scene).getCurrentLevelInfo().thumb << std::endl;
	
	backWidth = Globals::textureManager.get("paper").getWidth();
	backHeight = Globals::textureManager.get("paper").getHeight();

	columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(backWidth));
	rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(backHeight));
	
	JellyHellper::Instance()->LoadShaders();

	_jellyProjection = glm::ortho(-20.0f + 0, 0 + 20.0f, -4.2f + 4, 4 + 18.2f, -1.0f, 1.0f);
	_screenBounds = glm::vec4(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f);

	_fastCar = false;
	_slowCar = true;

	_showMap = false;

	//timer
	_dt = 0.0f;
	_time = 0.0f;
	//_timer = new Timer();

	_hitTime = 0.0f;
	_chassisHit = 0.0f;

	//jumping
	_isJumping = false;
	_inTheAir = false;
	_jumpStartPosition = 0.0f;
	_bestJumpLenght = 0.0f;

	mTransformMeter = 1.0f;
	mTransformMeterRechargeSpeed = 1.0f / 13.0f;
	mTransformMeterGrowDir = 0;

	//checkpoint
	_checkpoint = false;

	//ballon and tire initialization
	_haveBallon = false;
	_haveTire = false;

	_ballonActive = false;
	_tireActive = false;

	_ballonPressed = false;
	_tirePressed = false;

	_ballonTime = 0.0f;
	_tireTime = 0.0f;

	_ballonAABB.expandToInclude(Vector2((static_cast<float>(Application::Width) / 2.0f) + 128.0f + 64.0f, 450.0f + 64.0f));
	_ballonAABB.expandToInclude(Vector2((static_cast<float>(Application::Width) / 2.0f) + 128.0f - 64.0f, 450.0f - 64.0f));

	_tireAABB.expandToInclude(Vector2((static_cast<float>(Application::Width) / 2.0f) - 128.0f + 64.0f, 450.0f + 64.0f));
	_tireAABB.expandToInclude(Vector2((static_cast<float>(Application::Width) / 2.0f) - 128.0f - 64.0f, 450.0f - 64.0f));


	_world = new World();
	_levelManager = new LevelManager();
	_levelManager->SetAssetsLocation("Assets/Jelly/");
	_levelManager->LoadCarSkins("car_skins.xml");
	_levelManager->InitPhysic(_world);

	_carSkins = _levelManager->GetCarSkins();


	_sceneFile = SceneManager::Get().getSceneInfo(m_scene).getCurrentLevelInfo().file;
	_levelName = SceneManager::Get().getSceneInfo(m_scene).getCurrentLevelInfo().name;

	std::cout << _sceneFile << "  " << _levelName << std::endl;

	// filter callbacks (for sound playback)
	/*_world->setMaterialPairFilterCallback(0, 2, this);
	_world->setMaterialPairFilterCallback(1, 2, this);
	_world->setMaterialPairFilterCallback(4, 2, this);

	_world->setMaterialPairFilterCallback(0, 3, this);
	_world->setMaterialPairFilterCallback(1, 3, this);
	_world->setMaterialPairFilterCallback(4, 3, this);

	//ballon and tire collision
	_world->setMaterialPairFilterCallback(5, 2, this);
	_world->setMaterialPairFilterCallback(5, 3, this);*/

	//load level
	//_levelManager->LoadLevel(mWorld, _sceneFile, carFileName);
	_levelManager->LoadCompiledLevel(_world, _sceneFile, "Assets/Jelly/car_and_truck.car");

	//car
	_car = _levelManager->GetCar();

	//level elements
	_gameBodies = _levelManager->GetLevelBodies();

	//set map size
	_wholeMapPosition = _levelManager->GetWorldCenter();
	_wholeMapSize = _levelManager->GetWorldSize();
	_worldLimits = _levelManager->GetWorldLimits();

	//compute map view
	//_mapLimits
	//doing this at 2 am was not so easy :p
	if (_wholeMapSize.X > _wholeMapSize.Y){
		float sizeX = _wholeMapSize.X;
		float sizeY = _wholeMapSize.X  * 0.56f;

		float startX, startY;
		float endX, endY;

		if (sizeY < _wholeMapSize.Y){
			startY = _worldLimits.Min.Y - 5.0f;
			endY = _worldLimits.Max.Y + 5.0f;

			sizeX = ((_wholeMapSize.Y + 10.0f) / 0.56f);

			startX = _wholeMapPosition.X - (sizeX / 2.0f);
			endX = _wholeMapPosition.X + (sizeX / 2.0f);

		}else{
			startX = _worldLimits.Min.X;
			endX = _worldLimits.Max.X;

			startY = _wholeMapPosition.Y - (sizeY / 2.0f);
			endY = _wholeMapPosition.Y + (sizeY / 2.0f);
		}

		_mapLimits.Min.X = startX;
		_mapLimits.Min.Y = startY;
		_mapLimits.Max.X = endX;
		_mapLimits.Max.Y = endY;

	}else{
		float sizeX = _wholeMapSize.Y / 0.56f;
		float sizeY = _wholeMapSize.Y;

		float startX = 0.0f, startY = 0.0f;
		float endX = 0.0f, endY = 0.0f;

		if (sizeX < _wholeMapSize.X){
			//??
		}else{
			startY = _worldLimits.Min.Y;
			endY = _worldLimits.Max.Y;

			startX = _wholeMapPosition.X - (sizeX / 2.0f);
			endX = _wholeMapPosition.X + (sizeX / 2.0f);
		}

		_mapLimits.Min.X = startX;
		_mapLimits.Min.Y = startY;
		_mapLimits.Max.X = endX;
		_mapLimits.Max.Y = endY;
	}

	_wholeMapSize.X += 5;
	_wholeMapSize.Y = (_wholeMapSize.X *0.56f) / 2.0f;
	_wholeMapSize.X = _wholeMapSize.X / 2.0f;

	_levelTarget = _levelManager->GetLevelTarget();
	_levelLine = _levelManager->GetLevelLine();

	//_targetSprite->SetPosition(glm::vec2(_levelTarget.X, _levelTarget.Y));

	carBreakCount = 0;

	_newTimeRecord = false;
	_newJumpRecord = false;


	

	
}

JellyGame::~JellyGame() {
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

void JellyGame::fixedUpdate() {}

void JellyGame::update() {

	processInput();

	
}

void JellyGame::render() {

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

	

}

void JellyGame::resize(int deltaW, int deltaH) {
	columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(backWidth));
	rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(backHeight));
}

void JellyGame::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new JellyMenu(m_machine));
	}
}

void JellyGame::processInput() {

}