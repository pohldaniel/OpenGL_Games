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
	SceneManager::Get().getScene("scene").InitPhysic(m_world);
	SceneManager::Get().getScene("scene").loadCarSkins("Assets/Jelly/car_skins.xml");
	SceneManager::Get().getScene("scene").loadLevel("Assets/Jelly/Scenes_new/intro.scene");
	SceneManager::Get().getScene("scene").buildLevel(m_world, "Assets/Jelly/car_and_truck.car");
	
	const SkinInfo& skinInfo = SceneManager::Get().getScene("scene").getCurrentSkinInfo();
	m_car = SceneManager::Get().getScene("scene").GetCar();
	m_car->SetChassisTextures(skinInfo.skinTexture.chassisSmall, skinInfo.skinTexture.chassisBig);
	m_car->SetTireTextures(skinInfo.skinTexture.tireSmall, skinInfo.skinTexture.tireBig);

	m_gameBodies = SceneManager::Get().getScene("scene").GetLevelBodies();
	m_jellyProjection = glm::ortho(-20.0f + 0, 0 + 20.0f, -4.2f + 4, 4 + 18.2f, -1.0f, 1.0f);
	m_levelTarget = SceneManager::Get().getScene("scene").GetLevelTarget();

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

	SceneManager::Get().getScene("scene").loadSceneInfo("Assets/Jelly/scene_list.xml");
	m_copyWorld = new World();
	m_copyManager = new LevelManager();
	m_copyManager->SetAssetsLocation("Assets/Jelly/");

	/*const std::vector<SceneInfo>& levelInfos = SceneManager::Get().getScene("scene").getSceneInfos();
	for (auto& levelInfo : levelInfos) {
		//if (levelInfo.file.compare("cheez2.scene") == 0) {
			m_copyManager->LoadCompiledLevel(m_copyWorld, levelInfo.file, "Assets/Jelly/car_and_truck.car");
			Scene::SaveLevel("Assets/Jelly/Scenes_new/" + levelInfo.file, m_copyManager->GetObjectInfos(), m_copyManager->GetLevelBodies(), m_copyManager->_carPos, m_copyManager->GetLevelTarget(), m_copyManager->GetLevelLine(), levelInfo.name);
			m_copyManager->ClearLevel(m_copyWorld);
		//}
	}*/

	//m_copyManager->LoadCompiledLevel(m_copyWorld, "options_scene.scene", "Assets/Jelly/car_and_truck.car");
	//Scene::SaveLevel("Assets/Jelly/Scenes_new/options_scene.scene", m_copyManager->GetObjectInfos(), m_copyManager->GetLevelBodies(), m_copyManager->_carPos, m_copyManager->GetLevelTarget(), m_copyManager->GetLevelLine(), "Options");
	//m_copyManager->ClearLevel(m_copyWorld);

	//m_copyManager->LoadCompiledLevel(m_copyWorld, "elevel1.scene", "Assets/Jelly/car_and_truck.car");
	//Scene::SaveLevel("elevel1.scene", m_copyManager->GetObjectInfos(), m_copyManager->GetLevelBodies(), m_copyManager->_carPos, m_copyManager->GetLevelTarget(), m_copyManager->GetLevelLine(), "Initiation");
	//m_copyManager->ClearLevel(m_copyWorld);

	LoadingManagerSplitted::Get().addTask(new JellyIntro::LoadSceneAndThumbsTask(this, &JellyIntro::OnProcess, &JellyIntro::OnComplete));
	LoadingManagerSplitted::Get().startBackgroundThread();

	//SceneManager::Get().getScene("scene").loadLevel("Assets/Jelly/Scenes_new/test-scene5.scene");
}

JellyIntro::~JellyIntro() {	
	SceneManager::Get().getScene("scene").ClearLevel(m_world, m_gameBodies, m_car);
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
	if (m_car->getPosition().Y < SceneManager::Get().getScene("scene").GetLevelLine()){
		Vector2 pos = SceneManager::Get().getScene("scene").GetCarStartPos();
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
	SceneManager::Get().getScene("scene").loadSceneInfo("Assets/Jelly/scene_list.xml");
	TileSetManager::Get().getTileSet("thumbs").loadTileSetCpu(SceneManager::Get().getScene("scene").getThumbFiles());
}

void JellyIntro::OnComplete() {
	TileSetManager::Get().getTileSet("thumbs").loadTileSetGpu();
	//Spritesheet::Safe("thumbs",  TileSetManager::Get().getTileSet("thumbs").getAtlas());
}