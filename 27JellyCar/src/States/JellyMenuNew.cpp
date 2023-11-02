#include <engine/Fontrenderer.h>

#include "JellyMenuNew.h"
#include "Application.h"
#include "Globals.h"
#include "TileSet.h"


JellyMenuNew::JellyMenuNew(StateMachine& machine) : State(machine, CurrentState::JELLYMENU) {

	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);


	backWidth = Globals::textureManager.get("paper").getWidth();
	backHeight = Globals::textureManager.get("paper").getHeight();

	columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(backWidth));
	rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(backHeight));


	JellyHellper::Instance()->LoadShaders();

	_levelManager = new LevelManager();

	_levelManager->SetAssetsLocation("Assets/Jelly/");
	_levelManager->LoadAllScenes("scene_list.xml");
	_levelManager->LoadCarSkins("car_skins.xml");
	_levelManager->LoadScores("JellyScore.xml");

	//get scenes names
	_sceneFiles = _levelManager->GetScenes();

	//get first image
	//_levelImage = new Sprite("thumb", _levelManager->GetThumb(_sceneFiles[0]), _shader);
	//_levelImage->SetPosition(glm::vec2(_renderManager->GetWidth() / 2, 222));

	_carSkins = _levelManager->GetCarSkins();

	_world = new World();

	_levelManager->InitPhysic(_world);
	_levelManager->LoadLevel(_world, "menu.scene", "Assets/Jelly/car_and_truck.car");
	_levelManager->LoadCompiledLevel(_world, "menu.scene", "Assets/Jelly/car_and_truck.car");

	_car = _levelManager->GetCar();
	_car->SetChassisTextures(_levelManager->GetCarImage(_carSkins[0].chassisSmall), _levelManager->GetCarImage(_carSkins[0].chassisBig));
	_car->SetTireTextures(_levelManager->GetCarImage(_carSkins[0].tireSmall), _levelManager->GetCarImage(_carSkins[0].tireBig));

	_gameBodies = _levelManager->GetLevelBodies();
	_jellyProjection = glm::ortho(-20.0f + 0, 0 + 20.0f, -4.2f + 4, 4 + 18.2f, -1.0f, 1.0f);

	currentPosition = 0;
	columnStartPosition = 0;
	positionsInColumn = 5;

	carcurrentPosition = 0;
	carcolumnStartPosition = 0;
	carpositionsInColumn = 5;

	TileSetManager::Get().getTileSet("thumbs").loadTileSet({ 
															"Assets/Jelly/Thumbs/level-tut.png",
															"Assets/Jelly/Thumbs/easy1.png",
															"Assets/Jelly/Thumbs/level-02.png",
															"Assets/Jelly/Thumbs/easy2.png",
															"Assets/Jelly/Thumbs/crusher.png",
		
															"Assets/Jelly/Thumbs/easy3.png",
															"Assets/Jelly/Thumbs/level-06.png",
															"Assets/Jelly/Thumbs/easy4.png",
															"Assets/Jelly/Thumbs/snake.png",
															"Assets/Jelly/Thumbs/easy5.png",
	
															"Assets/Jelly/Thumbs/cave.png",
															"Assets/Jelly/Thumbs/easy6.png",
															"Assets/Jelly/Thumbs/ski-jump.png",
															"Assets/Jelly/Thumbs/easy7.png",
															"Assets/Jelly/Thumbs/catapult.png",
	
															"Assets/Jelly/Thumbs/easy8.png",
															"Assets/Jelly/Thumbs/boulders.png",
															"Assets/Jelly/Thumbs/easy9.png",
															"Assets/Jelly/Thumbs/level-05.png",
															"Assets/Jelly/Thumbs/easy10.png",
	
															"Assets/Jelly/Thumbs/level-04.png",
															"Assets/Jelly/Thumbs/normal2.png",
															"Assets/Jelly/Thumbs/level-03.png",
															"Assets/Jelly/Thumbs/normal3.png",
															"Assets/Jelly/Thumbs/wave.png", 
	
															"Assets/Jelly/Thumbs/normal4.png",
															"Assets/Jelly/Thumbs/level-01.png",
															"Assets/Jelly/Thumbs/normal5.png",
															"Assets/Jelly/Thumbs/level-08.png",
															"Assets/Jelly/Thumbs/normal6.png",
	
															"Assets/Jelly/Thumbs/level-07.png",
															"Assets/Jelly/Thumbs/normal7.png",
															"Assets/Jelly/Thumbs/rocket.png",
															"Assets/Jelly/Thumbs/normal8.png",
															"Assets/Jelly/Thumbs/big-wheel.png",
	
															"Assets/Jelly/Thumbs/normal9.png",
															"Assets/Jelly/Thumbs/waves.png",
															"Assets/Jelly/Thumbs/normal10.png",
															"Assets/Jelly/Thumbs/castle.png",
															"Assets/Jelly/Thumbs/hard1.png",
	
															"Assets/Jelly/Thumbs/blocks.png",
															"Assets/Jelly/Thumbs/hard2.png",
															"Assets/Jelly/Thumbs/spikes.png",
															"Assets/Jelly/Thumbs/hard3.png",
															"Assets/Jelly/Thumbs/grinder.png",
	
															"Assets/Jelly/Thumbs/hard4.png",
															"Assets/Jelly/Thumbs/homeward.png",
															"Assets/Jelly/Thumbs/hard5.png",
															"Assets/Jelly/Thumbs/revolver.png",
															"Assets/Jelly/Thumbs/hard6.png",

															"Assets/Jelly/Thumbs/loading.png",	
															"Assets/Jelly/Thumbs/hard7.png",
															"Assets/Jelly/Thumbs/derby.png",
															"Assets/Jelly/Thumbs/hard8.png",
															"Assets/Jelly/Thumbs/piter.png",

															"Assets/Jelly/Thumbs/hard9.png",
															"Assets/Jelly/Thumbs/jaklub.png",
															"Assets/Jelly/Thumbs/hard10.png",
															"Assets/Jelly/Thumbs/cogs.png",
															"Assets/Jelly/Thumbs/hard11.png",

															"Assets/Jelly/Thumbs/hard12.png" });

	m_thumbAtlas = TileSetManager::Get().getTileSet("thumbs").getAtlas();
	//Spritesheet::Safe("thumbs", m_atlas);
}

JellyMenuNew::~JellyMenuNew() {
	EventDispatcher::RemoveMouseListener(this);
	EventDispatcher::RemoveKeyboardListener(this);
}

void JellyMenuNew::fixedUpdate() {}

void JellyMenuNew::update() {

	processInput();

	for (int i = 0; i < 6; i++)
	{
		_world->update(0.004f);

		for (size_t i = 0; i < _gameBodies.size(); i++)
			_gameBodies[i]->Update(0.004f);

		_car->clearForces();
		_car->update(0.004f);
	}

	//reset car position
	if (_car->getPosition().Y < _levelManager->GetLevelLine())
	{
		Vector2 pos = _levelManager->GetCarStartPos();
		Vector2 scale = Vector2(1.0f, 1.0f);

		_car->getChassisBody()->setPositionAngle(pos, 0.0f, scale);
		_car->getTire(0)->setPositionAngle(pos, 0.0f, scale);
		_car->getTire(1)->setPositionAngle(pos, 0.0f, scale);
	}

}

void JellyMenuNew::render() {

	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	auto shader = Globals::shaderManager.getAssetPointer("quad_back");

	shader->use();
	Globals::textureManager.get("paper").bind(0);

	for (int y = 0; y < rows; y++){
		for (int x = 0; x < columns; x++) {

			int posx = (backWidth * x);
			int posy = Application::Height - (backHeight * (1 + y));

			shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(posx), static_cast<float>(posy), 0.0f) * Matrix4f::Scale(static_cast<float>(backWidth), static_cast<float>(backHeight), 1.0f));
			Globals::shapeManager.get("quad_aligned").drawRaw();
		}
	}
	Globals::textureManager.get("paper").unbind(0);

	shader->unuse();

	//menu level drawing
	
	//menu level
	for (size_t i = 0; i < _gameBodies.size(); i++){
		_gameBodies[i]->Draw(_jellyProjection);
	}

	//car
	_car->Draw(_jellyProjection);
	
	glEnable(GL_BLEND);

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

	//512 512
	//glm::vec2(239.0f, 194.0f), glm::vec2(74.0f, 38.0f) || start 
	//glm::vec2(159.0f, 193.0f), glm::vec2(74.0f, 38.0f) || select 

	//left = 239 / 512 || right = (239 + 74) / 512
	//bottom = (512 - (194 + 38))/ 512 || top = (512 - 194)/ 512
	
	
	Globals::textureManager.get("controls").bind(0);

	int posx = Application::Width / 2 - 40 ;
	int posy = 29;
	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(posx), static_cast<float>(posy), 0.0f)* Matrix4f::Scale(static_cast<float>(78), static_cast<float>(38), 1.0f));
	shader->loadVector("u_texRect", Vector4f(239.0f / 512.0f, (512.0f - (194.0f + 38.0f)) / 512.0f, (239.0f + 74.0f) / 512.0f, (512.0f - 194.0f) / 512.0f));

	Globals::shapeManager.get("quad_half").drawRaw();

	posx = Application::Width / 2 + 40 ;
	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(posx), static_cast<float>(posy), 0.0f)* Matrix4f::Scale(static_cast<float>(78), static_cast<float>(38), 1.0f));
	shader->loadVector("u_texRect", Vector4f(159.0f/ 512.0f, (512.0f - (193.0f + 38.0f))/ 512.0f, (159.0f + 74.0f)/512.0f, (512.0f - 193.0f) / 512.0f));

	Globals::shapeManager.get("quad_half").drawRaw();

	Globals::textureManager.get("controls").unbind(0);
	shader->unuse();

	Globals::spritesheetManager.getAssetPointer("jelly_font")->bind(0);

	int levelTextPositionY = Application::Height - ( (Application::Height / 2) - 156 - 30);

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_64"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_64").getWidth("Tutorial") / 2), levelTextPositionY, "Tutorial", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_64"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_64").getWidth("Tutorial") / 2), levelTextPositionY + 3, "Tutorial", Vector4f(1.0f,  0.65f, 0.0f, 1.0f));

	int timePositionX = (Application::Width / 2) - (272 / 2);
	int jumpPositionX = (Application::Width / 2) + (272 / 2);
	int textPosY = Application::Height - ((Application::Height / 2) + 30);

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), timePositionX , textPosY, "999.00s", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), timePositionX , textPosY + 2, "999.00s", Vector4f(0.0f, 0.84f, 0.0f, 1.0f));

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), jumpPositionX - Globals::fontManager.get("jelly_32").getWidth("0.00m"), textPosY, "0.00m", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), jumpPositionX - Globals::fontManager.get("jelly_32").getWidth("0.00m"), textPosY + 2, "0.00m", Vector4f(0.71f, 0.16f, 0.18f, 1.0f));

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - 90 - Globals::fontManager.get("jelly_32").getWidth("Options")), 10, "Options", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 90), 10, "Exit", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().drawBuffer();
	Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);
	glDisable(GL_BLEND);

}

void JellyMenuNew::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void JellyMenuNew::OnMouseButtonDown(Event::MouseButtonEvent& event) {

}

void JellyMenuNew::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void JellyMenuNew::resize(int deltaW, int deltaH) {
	columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(backWidth));
	rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(backHeight));
}

void JellyMenuNew::processInput() {
	Keyboard &keyboard = Keyboard::instance();
	
	if (keyboard.keyPressed(Keyboard::KEY_UP)){
		currentPosition--;

		if (currentPosition < 0){
			currentPosition = 0;
		}
		//_audioHelper->PlayHitSound();
	}

	if (keyboard.keyPressed(Keyboard::KEY_DOWN)){
		currentPosition++;

		if (currentPosition >= _sceneFiles.size()){
			currentPosition = _sceneFiles.size() - 1;
		}
		//_audioHelper->PlayHitSound();
	}

	if (keyboard.keyPressed(Keyboard::KEY_D)){
		carcurrentPosition++;

		if (carcurrentPosition >= _carSkins.size()){
			carcurrentPosition = _carSkins.size() - 1;
		}

		_car->SetChassisTextures(_levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisSmall), _levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisBig));
		_car->SetTireTextures(_levelManager->GetCarImage(_carSkins[carcurrentPosition].tireSmall), _levelManager->GetCarImage(_carSkins[carcurrentPosition].tireBig));
	}

	if (keyboard.keyPressed(Keyboard::KEY_A)){
		carcurrentPosition--;

		if (carcurrentPosition < 0){
			carcurrentPosition = 0;
		}

		_car->SetChassisTextures(_levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisSmall), _levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisBig));
		_car->SetTireTextures(_levelManager->GetCarImage(_carSkins[carcurrentPosition].tireSmall), _levelManager->GetCarImage(_carSkins[carcurrentPosition].tireBig));
	}

	if (keyboard.keyPressed(Keyboard::KEY_S)){
		_car->Transform();
	}

	if (!keyboard.keyDown(Keyboard::KEY_LEFT) && !keyboard.keyDown(Keyboard::KEY_RIGHT))
		_car->setTorque(0);

	if (keyboard.keyDown(Keyboard::KEY_LEFT)){
		_car->setTorque(-1);
	}

	if (keyboard.keyDown(Keyboard::KEY_RIGHT)){
		_car->setTorque(1);
	}

	//chasis torque
	_car->mChassis->torque = 0.0f;

	if (keyboard.keyDown(Keyboard::KEY_Q)){
		_car->mChassis->torque = -1.0f;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)){
		_car->mChassis->torque = 1.0f;
	}
}