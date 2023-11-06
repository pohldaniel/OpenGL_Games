#include <engine/Fontrenderer.h>

#include "JellyIntro.h"
#include "JellyMenu.h"
#include "Application.h"
#include "Globals.h"
#include "JellyHelper.h"

JellyIntro::JellyIntro(StateMachine& machine) : State(machine, CurrentState::JELLYINTRO) {

	backWidth = Globals::textureManager.get("paper").getWidth();
	backHeight = Globals::textureManager.get("paper").getHeight();

	columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(backWidth));
	rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(backHeight));


	JellyHellper::Instance()->LoadShaders();
	_world = new World();

	_levelManager = new LevelManager();
	_levelManager->SetAssetsLocation("Assets/Jelly/");
	_levelManager->LoadCarSkins("car_skins.xml");
	_levelManager->InitPhysic(_world);
	_levelManager->LoadCompiledLevel(_world, "intro.scene", "Assets/Jelly/car_and_truck.car");

	_carSkins = _levelManager->GetCarSkins();

	_car = _levelManager->GetCar();
	_car->SetChassisTextures(_levelManager->GetCarImage(_carSkins[0].chassisSmall), _levelManager->GetCarImage(_carSkins[0].chassisBig));
	_car->SetTireTextures(_levelManager->GetCarImage(_carSkins[0].tireSmall), _levelManager->GetCarImage(_carSkins[0].tireBig));

	_gameBodies = _levelManager->GetLevelBodies();
	_jellyProjection = glm::ortho(-20.0f + 0, 0 + 20.0f, -4.2f + 4, 4 + 18.2f, -1.0f, 1.0f);

	_levelTarget = _levelManager->GetLevelTarget();
	_end = false;

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
}

JellyIntro::~JellyIntro() {
	if (_levelManager != 0){
		//clear level
		_levelManager->ClearLevel(_world);

		//remove level manager
		delete _levelManager;
	}

	//remove physic world
	delete _world;
	_gameBodies.clear();
}

void JellyIntro::fixedUpdate() {}

void JellyIntro::update() {

	if (_end){
		//manager->PopState();
		return;
	}

	_car->setTorque(1);

	//Update physic
	for (int i = 0; i < 6; i++){
		_world->update(0.004f);

		for (size_t i = 0; i < _gameBodies.size(); i++)
			_gameBodies[i]->Update(0.004f);

		_car->clearForces();
		_car->update(0.004f);
	}

	//reset car position
	if (_car->getPosition().Y < _levelManager->GetLevelLine()){
		Vector2 pos = _levelManager->GetCarStartPos();
		Vector2 scale = Vector2(1.0f, 1.0f);

		_car->getChassisBody()->setPositionAngle(pos, 0.0f, scale);
		_car->getTire(0)->setPositionAngle(pos, 0.0f, scale);
		_car->getTire(1)->setPositionAngle(pos, 0.0f, scale);
	}

	bool enterMenu = false;

	//end of scene
	if (_car->getChassisBody()->getAABB().contains(_levelTarget)){
		if (_car->getChassisBody()->contains(_levelTarget)){
			enterMenu = true;
		}
	}

	for (int i = 0; i < 2; i++){
		if (_car->getTire(i)->getAABB().contains(_levelTarget)){
			if (_car->getTire(i)->contains(_levelTarget)){
				enterMenu = true;
			}
		}
	}

	if (enterMenu){
		_end = true;
		//_audioHelper->StopEngineSound();
		auto shader = Globals::shaderManager.getAssetPointer("quad_back");
		shader->use();
		shader->loadInt("u_texture", 0);
		shader->unuse();

		shader = Globals::shaderManager.getAssetPointer("quad");
		shader->use();
		shader->loadInt("u_texture", 0);
		shader->unuse();

		m_isRunning = false;
		m_machine.addStateAtBottom(new JellyMenu(m_machine));
	}
}

void JellyIntro::render() {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto shader = Globals::shaderManager.getAssetPointer("quad_back");

	shader->use();
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < columns; x++) {

			int posx = (backWidth * x);
			int posy = Application::Height - (backHeight * (1 + y));

			shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(posx), static_cast<float>(posy), 0.0f) * Matrix4f::Scale(static_cast<float>(backWidth), static_cast<float>(backHeight), 1.0f));
			Globals::shapeManager.get("quad_aligned").drawRaw();
		}
	}

	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	
	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(Application::Width / 2), static_cast<float>(Application::Height  - 200), 0.0f) * Matrix4f::Scale(static_cast<float>(Globals::textureManager.get("logo").getWidth()), static_cast<float>(Globals::textureManager.get("logo").getHeight()), 1.0f));
	Globals::shapeManager.get("quad_half").drawRaw();
	shader->unuse();
	
	for (size_t i = 0; i < _gameBodies.size(); i++) {
		_gameBodies[i]->Draw(_jellyProjection);
	}
	_car->Draw(_jellyProjection);

}

void JellyIntro::resize(int deltaW, int deltaH) {
	columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(backWidth));
	rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(backHeight));
}