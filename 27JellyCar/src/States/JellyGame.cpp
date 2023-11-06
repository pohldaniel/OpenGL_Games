#include <engine/Fontrenderer.h>

#include "JellyGame.h"
#include "JellyMenu.h"
#include "JellyPause.h"
#include "JellyFinish.h"
#include "JellyDialog.h"

#include "Application.h"
#include "Globals.h"
#include "JellyHelper.h"
#include "SceneManager.h"

JellyGame::JellyGame(StateMachine& machine, std::string scene) : State(machine, CurrentState::JELLYGAME) {
	EventDispatcher::AddKeyboardListener(this);

	m_scene = scene;

	backWidth = Globals::textureManager.get("paper").getWidth();
	backHeight = Globals::textureManager.get("paper").getHeight();

	columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(backWidth));
	rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(backHeight));
	
	JellyHellper::Instance()->LoadShaders();
	_shaderManager = ShaderManager::Instance();

	_projection = glm::ortho(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, -1.0f, 1.0f);
	_jellyProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -1.0f, 1.0f);
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

	_shader = _shaderManager->LoadFromFile("sprite", "Assets/Shaders/sprite", "Assets/Shaders/sprite", Textured);
	_backSprite = new Sprite("paper", "Assets/Jelly/Texture/paper.png", _shader);

	_targetSprite = new Sprite("target", "Assets/Jelly/Texture/finish.png", _shader);
	_targetSprite->SetScale(glm::vec2(0.04f, 0.04f));

	_tireSprite = new Sprite("tireSprite", "Assets/Jelly/Texture/tire.png", _shader);
	_tireSprite->SetScale(glm::vec2(0.04f, 0.04f));

	_ballonSprite = new Sprite("ballonSprite", "Assets/Jelly/Texture/ballon.png", _shader);
	_ballonSprite->SetScale(glm::vec2(0.04f, 0.04f));

	_tireSpriteBack = new Sprite("tireBack", "Assets/Jelly/Texture/roundBackRed.png", _shader);
	_ballonSpriteBack = new Sprite("ballonBack", "Assets/Jelly/Texture/roundBack.png", _shader);

	_transformMeter = new Sprite("transformmeter", "Assets/Jelly/Texture/transform_meter.png", _shader);
	_transformMeter->SetPosition(glm::vec2(128, 40));

	_sceneFile = SceneManager::Get().getSceneInfo(m_scene).getCurrentLevelInfo().file;
	_levelName = SceneManager::Get().getSceneInfo(m_scene).getCurrentLevelInfo().name;

	_world = new World();
	_levelManager = new LevelManager();
	_levelManager->SetAssetsLocation("Assets/Jelly/");
	_levelManager->LoadCarSkins("car_skins.xml");
	_levelManager->InitPhysic(_world);
	_levelManager->LoadCompiledLevel(_world, _sceneFile, "Assets/Jelly/car_and_truck.car");


	const SkinInfo& skinInfo = SceneManager::Get().getSceneInfo("scene").getCurrentSkinInfo();
	_car = _levelManager->GetCar();
	_car->SetChassisTextures(_levelManager->GetCarImage(skinInfo.chassisSmall), _levelManager->GetCarImage(skinInfo.chassisBig));
	_car->SetTireTextures(_levelManager->GetCarImage(skinInfo.tireSmall), _levelManager->GetCarImage(skinInfo.tireBig));

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

	_targetSprite->SetPosition(glm::vec2(_levelTarget.X, _levelTarget.Y));

	carBreakCount = 0;

	_newTimeRecord = false;
	_newJumpRecord = false;


	m_mainRT.create(Application::Width, Application::Height);
	m_mainRT.attachTexture2D(AttachmentTex::RGBA);
	m_mainRT.attachRenderbuffer(AttachmentRB::DEPTH24);

	
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

	_dt = m_dt;
	_time += _dt;
	_hitTime += _dt;

	//hit info - zero before update/callback event
	_chassisHit = 0;

	//jumping info - true before update/callback event
	_isJumping = true;

	//Update physic
	for (int i = 0; i < 6; i++){
		_world->update(0.004f);

		for (size_t i = 0; i < _gameBodies.size(); i++)
			_gameBodies[i]->Update(0.004f);

		_car->clearForces();
		_car->update(0.004f);

		UpdateTransformMeter(0.004f);
	}


	if (_ballonActive){
		_ballonTime -= _dt;

		if (_ballonTime <= 0.0f){
			_car->UseBallon(false);
			_ballonActive = false;
			_haveBallon = false;
		}
	}

	if (_tireActive){
		_tireTime -= _dt;

		if (_tireTime <= 0.0f){
			_car->UseNearestGracity(false);
			_tireActive = false;
			_haveTire = false;
		}
	}

	//update camera*
	//screen ratio 0.56
	if (_showMap){
		_jellyProjection = glm::ortho(_mapLimits.Min.X, _mapLimits.Max.X, _mapLimits.Min.Y, _mapLimits.Max.Y, -1.0f, 1.0f);
	}else{
		_jellyProjection = glm::ortho(-20.0f + _car->getPosition().X, _car->getPosition().X + 20.0f, -11.2f + _car->getPosition().Y, _car->getPosition().Y + 11.2f, -1.0f, 1.0f);
	}

	//check level ending
	if (_car->getChassisBody()->getAABB().contains(_levelTarget)){
		if (_car->getChassisBody()->contains(_levelTarget)){
			//_gamePlayState = GamePlayState::Finish;
			//_audioHelper->StopEngineSound();

			/*if (_time < _menuLevelManager->GetTime(_levelName)){
				_newTimeRecord = true;

				_menuLevelManager->SetTime(_levelName, _time);
				_menuLevelManager->SaveScores("JellyScore.xml");
			}

			if (_bestJumpLenght > _menuLevelManager->GetJump(_levelName)){
				_newJumpRecord = true;

				_menuLevelManager->SetJump(_levelName, _bestJumpLenght);
				_menuLevelManager->SaveScores("JellyScore.xml");
			}*/
			m_machine.addStateAtTop(new JellyFinish(m_machine, m_mainRT, _newJumpRecord, _newTimeRecord));
			return;
		}
	}

	for (int i = 0; i < 2; i++){
		if (_car->getTire(i)->getAABB().contains(_levelTarget)){
			if (_car->getTire(i)->contains(_levelTarget)){
				/*_gamePlayState = GamePlayState::Finish;
				_audioHelper->StopEngineSound();

				if (_time < _menuLevelManager->GetTime(_levelName)){
					_newTimeRecord = true;

					_menuLevelManager->SetTime(_levelName, _time);
					_menuLevelManager->SaveScores("JellyScore.xml");
				}

				if (_bestJumpLenght > _menuLevelManager->GetJump(_levelName)){
					_newJumpRecord = true;

					_menuLevelManager->SetJump(_levelName, _bestJumpLenght);
					_menuLevelManager->SaveScores("JellyScore.xml");
				}*/

				m_machine.addStateAtTop(new JellyFinish(m_machine, m_mainRT, _newJumpRecord, _newTimeRecord));
				return;
			}
		}
	}

	//check out of bounds
	if (_car->getPosition().Y < _levelLine){
		if (_checkpoint){
			Vector2 pos = Vector2(_checkpointPosition.x, _checkpointPosition.y);
			Vector2 scale = Vector2(1.0f, 1.0f);

			_car->getChassisBody()->setVelocity(Vector2(0, 0));
			_car->getTire(0)->setVelocity(Vector2(0, 0));
			_car->getTire(1)->setVelocity(Vector2(0, 0));

			_car->getChassisBody()->setPositionAngle(pos, 0.0f, scale);
			_car->getTire(0)->setPositionAngle(Vector2(_checkpointPosition.x + 1.5f, _checkpointPosition.y - 0.3f), 0.0f, scale);
			_car->getTire(1)->setPositionAngle(Vector2(_checkpointPosition.x - 1.3f, _checkpointPosition.y - 0.3f), 0.0f, scale);
		}else{
			//_gamePlayState = GamePlayState::OutOfBounds;
			//_audioHelper->StopEngineSound();
			m_machine.addStateAtTop(new JellyDialog(m_machine, m_mainRT, "Out of level"));
			return;
		}
	}

	//check car broken
	AABB chassisAABB = _car->getChassisBody()->getAABB();
	Vector2 chassisSize = chassisAABB.Max - chassisAABB.Min;
	if ((fabsf(chassisSize.X) > 17.0f) || (fabsf(chassisSize.Y) > 17.0f)){
		//_gamePlayState = GamePlayState::CarBroken;
		//_audioHelper->StopEngineSound();
		m_machine.addStateAtTop(new JellyDialog(m_machine, m_mainRT, "Car broken"));
		return;
	}

	//car broken
	if (_world->getPenetrationCount() > 20){
		carBreakCount++;
		if (carBreakCount > 5){
			//_gamePlayState = GamePlayState::CarBroken;
			//_audioHelper->StopEngineSound();
			m_machine.addStateAtTop(new JellyDialog(m_machine, m_mainRT, "Car broken"));
			return;
		}
	}

	//jumping
	if (_isJumping && !_inTheAir){
		//we just started jumping
		_jumpStartPosition = _car->getPosition().X;
		_inTheAir = true;
	}

	if (!_isJumping && _inTheAir){
		//end of jump - check if there is new jump record
		float jumpLenght = fabsf(_jumpStartPosition - _car->getPosition().X);

		if (jumpLenght > _bestJumpLenght){
			_bestJumpLenght = jumpLenght;
		}

		_inTheAir = false;
	}

	//hit sound
	if (_chassisHit > 0.0f){
		if ((_chassisHit > 3.0f) && (_hitTime > 0.3f)){
			//play hit sound
			//_audioHelper->PlayHitSound();

			//set timer
			_hitTime = 0.0f;
		}
	}
}

void JellyGame::render() {

	renderScene();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	shader->loadBool("u_flip", false);
	m_mainRT.bindColorTexture();
	Globals::shapeManager.get("quad").drawRaw();
	m_mainRT.unbindColorTexture();
	shader->unuse();
}

void JellyGame::renderScene() {
	m_mainRT.bindWrite();
	glClearBufferfv(GL_COLOR, 0, std::vector<float>{0.0f, 0.0f, 0.0f, 0.0f}.data());
	glClearBufferfv(GL_DEPTH, 0, &std::vector<float>{1.0f}[0]);

	_backSprite->SetScale(glm::vec2(0.1f, 0.1f));
	if (_showMap) {
		float x = _mapLimits.Min.X - 51.0f;
		float y = _mapLimits.Min.Y - 51.0f;

		for (float xx = x; xx <= _mapLimits.Max.X + 51.0f; xx += 51.0f) {
			for (float yy = y; yy <= _mapLimits.Max.Y + 51.0f; yy += 51.0f) {
				_backSprite->SetPosition(glm::vec2(xx, yy));
				_backSprite->Draw(_jellyProjection);
			}
		}
	}
	else {
		float x = _worldLimits.Min.X - 51.0f;
		float y = _worldLimits.Min.Y - 51.0f;

		for (float xx = x; xx <= _worldLimits.Max.X + 51.0f; xx += 51.0f) {
			for (float yy = y; yy <= _worldLimits.Max.Y + 51.0f; yy += 51.0f) {
				_backSprite->SetPosition(glm::vec2(xx, yy));
				_backSprite->Draw(_jellyProjection);
			}
		}
	}

	//camera aabb
	Vector2 camMin = _car->getPosition() - Vector2(20.0f, 11.2f);
	Vector2 camMax = _car->getPosition() + Vector2(20.0f, 11.2f);
	JellyPhysics::AABB camAABB(camMin, camMax);

	//ignore aabb
	Vector2 ignoreMin = _car->getPosition() - Vector2(40.0f, 22.4f);
	Vector2 ignoreMax = _car->getPosition() + Vector2(40.0f, 22.4f);
	JellyPhysics::AABB ignoreAABB(ignoreMin, ignoreMax);

	//render level bodies
	for (size_t i = 0; i < _gameBodies.size(); i++) {
		if (_showMap) {
			_gameBodies[i]->Draw(_jellyProjection);

			if (ignoreAABB.intersects(_gameBodies[i]->GetIgnoreAABB())) {
				_gameBodies[i]->SetIgnore(false);
			}
			else {
				_gameBodies[i]->SetIgnore(true);
			}

		}
		else {

			if (ignoreAABB.intersects(_gameBodies[i]->GetIgnoreAABB())) {
				_gameBodies[i]->SetIgnore(false);

				if (_gameBodies[i]->GetIgnoreAABB().intersects(camAABB)) {
					_gameBodies[i]->Draw(_jellyProjection);
				}
			}
			else {
				_gameBodies[i]->SetIgnore(true);
			}
		}
	}

	//car
	_car->Draw(_jellyProjection);

	//ballon - tire
	for (size_t i = 0; i < _gameBodies.size(); i++) {
		if (!_gameBodies[i]->GetBody()->getDisable()) {
			if (_gameBodies[i]->GetName() == "itemballoon") {
				//draw ballon
				_ballonSprite->SetScale(glm::vec2(0.02f, -0.02f));
				_ballonSprite->SetPosition(_gameBodies[i]->GetStartPosition());
				_ballonSprite->Draw(_jellyProjection);
			}
			else if (_gameBodies[i]->GetName() == "itemstick") {
				_tireSprite->SetScale(glm::vec2(0.02f, 0.02f));
				_tireSprite->SetPosition(_gameBodies[i]->GetStartPosition());
				_tireSprite->Draw(_jellyProjection);
			}
		}
	}

	if (!_showMap && _haveBallon && _ballonTime > 0.0f) {
		float step = 100.0f / 30.0f;
		float scale = (_ballonTime * step) / 100.0f;

		_ballonSpriteBack->SetScale(glm::vec2(scale, scale));
		_ballonSpriteBack->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
		_ballonSpriteBack->SetPosition(glm::vec2((Application::Width / 2) + 128, 450));
		_ballonSpriteBack->Draw(_projection);

		_ballonSprite->SetScale(glm::vec2(scale, scale));
		//_ballonSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
		_ballonSprite->SetPosition(glm::vec2((Application::Width / 2) + 128, 450));
		_ballonSprite->Draw(_projection);
	}

	//_transformMeter
	if (mTransformMeter >= 0.0f) {
		float test = 1.0f - mTransformMeter;
		_transformMeter->SetPosition(glm::vec2(128 - (test * 64.0f), 40.0f));
		_transformMeter->SetSize(glm::vec2(128 * mTransformMeter, 32));
		_transformMeter->Draw(_projection);
	}

	_targetSprite->Draw(_jellyProjection);


	Globals::spritesheetManager.getAssetPointer("jelly_font")->bind(0);
	std::string time = Fontrenderer::FloatToString(_time, 2);
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width - 125), Application::Height - 52, time, Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width - 125), Application::Height - 50, time, Vector4f(0.0f, 0.84f, 0.0f, 1.0f));

	std::string jump = Fontrenderer::FloatToString(_bestJumpLenght, 2);
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width - 125), Application::Height - 82, jump, Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width - 125), Application::Height - 80, jump, Vector4f(0.71f, 0.16f, 0.18f, 1.0f));
	Fontrenderer::Get().drawBuffer();

	Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);

	m_mainRT.unbindWrite();
}

void JellyGame::resize(int deltaW, int deltaH) {
	columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(backWidth));
	rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(backHeight));

	m_mainRT.resize(Application::Width, Application::Height);

	if (m_machine.getStates().top()->getCurrentState() != CurrentState::JELLYGAME) {
		renderScene();
	}
	
}

void JellyGame::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new JellyMenu(m_machine));
	}
}

void JellyGame::processInput() {
	Keyboard &keyboard = Keyboard::instance();

	if (keyboard.keyPressed(Keyboard::KEY_ENTER)){
		//_gamePlayState = GamePlayState::Paused;
		//_audioHelper->StopEngineSound();
		//return;

		m_machine.addStateAtTop(new JellyPause(m_machine, m_mainRT));		
		return;
	}

	if (keyboard.keyPressed(Keyboard::KEY_W)) {
		_showMap = !_showMap;
	}

	if (!keyboard.keyDown(Keyboard::KEY_LEFT) && !keyboard.keyDown(Keyboard::KEY_RIGHT))
		_car->setTorque(0);

	if (keyboard.keyDown(Keyboard::KEY_LEFT)) {
		_car->setTorque(-1);
	}

	if (keyboard.keyDown(Keyboard::KEY_RIGHT)) {
		_car->setTorque(1);
	}

	//chasis torque
	_car->mChassis->torque = 0.0f;

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		_car->mChassis->torque = -1.0f;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		_car->mChassis->torque = 1.0f;
	}

	if (keyboard.keyPressed(Keyboard::KEY_S)) {
		if (mTransformMeter > 0.0f){
			if (_car->Transform())
				mTransformMeterGrowDir = (mTransformMeterGrowDir >= 0) ? -1 : 1;
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_D)){
		_ballonActive = !_ballonActive;
		_car->UseBallon(_ballonActive);
	}

	if (keyboard.keyPressed(Keyboard::KEY_A)){
		_tireActive = !_tireActive;
		_car->UseNearestGracity(_tireActive);
	}

	if (keyboard.keyDown(Keyboard::KEY_LEFT) || keyboard.keyDown(Keyboard::KEY_RIGHT)){
		if (_car->getVelocity().length() > 2.0f && _slowCar){
			//_audioHelper->PlayFastEngine();
			_slowCar = false;
			_fastCar = true;
		}

	}else{
		if (_fastCar){
			//_audioHelper->PlaySlowEngine();
			_slowCar = true;
			_fastCar = false;
		}
	}

	//touch support
	//tire and ballon
	/*if (_haveBallon || _haveTire){
		if (_inputHelper->GetTouchCount() > 0){
			glm::vec2 touch = _inputHelper->TouchToScreen(_screenBounds, _inputHelper->GetTouchPosition(0));

			//check if pressed ballon trigger
			if (_haveBallon && _ballonAABB.contains(Vector2(touch.x, touch.y))){
				if (!_ballonPressed){
					//change ballon state
					_ballonActive = !_ballonActive;

					//activate or deactivate ballon
					_car->UseBallon(_ballonActive);

					_ballonPressed = true;
				}
			}

			//check if pressed tire trigger
			if (_haveTire && _tireAABB.contains(Vector2(touch.x, touch.y))){
				if (!_tirePressed){
					//change ballon state
					_tireActive = !_tireActive;

					//activate or deactivate ballon
					_car->UseNearestGracity(_tireActive);

					_tirePressed = true;
				}
			}
		}else{
			_ballonPressed = false;
			_tirePressed = false;
		}
	}*/
}

void JellyGame::UpdateTransformMeter(float dt){
	// transform meter update.
	if (mTransformMeterGrowDir != 0){
		//float oldValue = mTransformMeter;
		mTransformMeter += (mTransformMeterRechargeSpeed * dt * (float)mTransformMeterGrowDir);

		if (mTransformMeter <= 0.0f){
			if (_car->TransformStatus != Car::Normal)
				_car->Transform();
		}

		if (mTransformMeter <= -0.2f){
			mTransformMeter = -0.2f;
			mTransformMeterGrowDir = 1;
		}

		if ((mTransformMeter > 1.0f)){
			mTransformMeter = 1.0f;
			mTransformMeterGrowDir = 0;
		}
	}
}