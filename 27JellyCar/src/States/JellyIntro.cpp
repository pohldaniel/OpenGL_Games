#include <engine/Fontrenderer.h>
#include <filesystem>
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

	//SceneManager::Get().getScene("scene").configure("Assets/Jelly/Scenes_origin/", false);
	//SceneManager::Get().getScene("scene").configure("Assets/Jelly/Scenes_new/", false);
	SceneManager::Get().getScene("scene").configure("Assets/Jelly/Scenes_compiled_new/", true);

	SceneManager::Get().getScene("scene").InitPhysic(m_world);
	SceneManager::Get().getScene("scene").loadCarSkins("Assets/Jelly/car_skins.xml");	
	SceneManager::Get().getScene("scene").loadLevel("intro.scene");
	SceneManager::Get().getScene("scene").buildLevel(m_world, m_gameBodies);
	SceneManager::Get().getScene("scene").buildCar(m_world, m_car, "Assets/Jelly/car_and_truck.car");

	const SkinInfo& skinInfo = SceneManager::Get().getScene("scene").getCurrentSkinInfo();
	m_car->SetChassisTextures(skinInfo.skinTexture.chassisSmall, skinInfo.skinTexture.chassisBig);
	m_car->SetTireTextures(skinInfo.skinTexture.tireSmall, skinInfo.skinTexture.tireBig);

	m_jellyProjection = glm::ortho(-20.0f + 0, 0 + 20.0f, -4.2f + 4, 4 + 18.2f, -1.0f, 1.0f);
	m_levelTarget = SceneManager::Get().getScene("scene").getLevelTarget();

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
}

JellyIntro::~JellyIntro() {	
	Scene::ClearLevel(m_world, m_gameBodies, m_car);
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
	if (m_car->getPosition().Y < SceneManager::Get().getScene("scene").getLevelLine()){
		Vector2 pos = SceneManager::Get().getScene("scene").getCarStartPos();
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
	SceneManager::Get().getScene("scene").loadSceneInfo(SceneManager::Get().getScene("scene").getScenesPath() + "scene_list.xml");
	TileSetManager::Get().getTileSet("thumbs").loadTileSetCpu(SceneManager::Get().getScene("scene").getThumbFiles());
}

void JellyIntro::OnComplete() {
	TileSetManager::Get().getTileSet("thumbs").loadTileSetGpu();
	//Spritesheet::Safe("thumbs",  TileSetManager::Get().getTileSet("thumbs").getAtlas());
}