#include <engine/Fontrenderer.h>

#include "JellyIntro.h"
#include "JellyMenu.h"
#include "Application.h"
#include "Globals.h"
#include "JellyHelper.h"
#include "TileSet.h"
#include "Loadingmanager.h"

JellyIntro::JellyIntro(StateMachine& machine) : State(machine, States::JELLYINTRO) {

	m_backWidth = Globals::textureManager.get("paper").getWidth();
	m_backHeight = Globals::textureManager.get("paper").getHeight();

	m_columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(m_backWidth));
	m_rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(m_backHeight));


	JellyHellper::Instance()->LoadShaders();
	m_world = new World();

	m_levelManager = new LevelManager();
	m_levelManager->SetAssetsLocation("Assets/Jelly/");
	m_levelManager->InitPhysic(m_world);
	m_levelManager->LoadCompiledLevel(m_world, "intro.scene", "Assets/Jelly/car_and_truck.car");

	SceneManager::Get().getSceneInfo("scene").loadCarSkins("Assets/Jelly/car_skins.xml");
	const SkinInfo& skinInfo = SceneManager::Get().getSceneInfo("scene").getCurrentSkinInfo();
	m_car = m_levelManager->GetCar();
	m_car->SetChassisTextures(skinInfo.skinTexture.chassisSmall, skinInfo.skinTexture.chassisBig);
	m_car->SetTireTextures(skinInfo.skinTexture.tireSmall, skinInfo.skinTexture.tireBig);

	m_gameBodies = m_levelManager->GetLevelBodies();
	m_jellyProjection = glm::ortho(-20.0f + 0, 0 + 20.0f, -4.2f + 4, 4 + 18.2f, -1.0f, 1.0f);
	m_levelTarget = m_levelManager->GetLevelTarget();

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

	LoadingManagerSplitted::Get().addTask(new JellyIntro::LoadSceneAndThumbsTask(this, &JellyIntro::OnProcess, &JellyIntro::OnComplete));
	LoadingManagerSplitted::Get().startBackgroundThread();

	m_copyWorld = new World();
	m_copyManager = new LevelManager();
	m_copyManager->SetAssetsLocation("Assets/Jelly/");
	m_copyManager->LoadLevel(m_copyWorld, "tut-scene.scene", "Assets/Jelly/car_and_truck.car");

	SceneInfo::SaveLevel("tut.scene", m_copyManager->GetObjectInfos(), m_copyManager->GetLevelBodies(), m_copyManager->GetLevelTarget(), m_copyManager->GetLevelLine(), m_copyManager->m_currentName);
}

JellyIntro::~JellyIntro() {
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

void JellyIntro::fixedUpdate() {}

void JellyIntro::update() {

	
	LoadingManagerSplitted::Get().update();

	m_car->setTorque(1);

	//Update physic
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

	bool enterMenu = false;

	//end of scene
	if (m_car->getChassisBody()->getAABB().contains(m_levelTarget)){
		if (m_car->getChassisBody()->contains(m_levelTarget)){
			enterMenu = true;
		}
	}

	for (int i = 0; i < 2; i++){
		if (m_car->getTire(i)->getAABB().contains(m_levelTarget)){
			if (m_car->getTire(i)->contains(m_levelTarget)){
				enterMenu = true;
			}
		}
	}

	if (enterMenu){
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
	
	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(Application::Width / 2), static_cast<float>(Application::Height  - 200), 0.0f) * Matrix4f::Scale(static_cast<float>(Globals::textureManager.get("logo").getWidth()), static_cast<float>(Globals::textureManager.get("logo").getHeight()), 1.0f));
	Globals::shapeManager.get("quad_half").drawRaw();
	shader->unuse();
	
	for (size_t i = 0; i < m_gameBodies.size(); i++) {
		m_gameBodies[i]->Draw(m_jellyProjection);
	}

	m_car->Draw(m_jellyProjection);
}

void JellyIntro::resize(int deltaW, int deltaH) {
	m_columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(m_backWidth));
	m_rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(m_backHeight));
}

void JellyIntro::OnProcess() {
	SceneManager::Get().getSceneInfo("scene").loadLevelInfo("Assets/Jelly/scene_list.xml");
	TileSetManager::Get().getTileSet("thumbs").loadTileSetCpu(SceneManager::Get().getSceneInfo("scene").getThumbFiles());
}

void JellyIntro::OnComplete() {
	TileSetManager::Get().getTileSet("thumbs").loadTileSetGpu();
	//Spritesheet::Safe("thumbs",  TileSetManager::Get().getTileSet("thumbs").getAtlas());
}