#include <time.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Tutorial.h"
#include "Constants.h"
#include "ResourceManager.h"
#include "Physics.h"
#include "Player.h"
#include "Application.h"

Tutorial::Tutorial(StateMachine& machine) : State(machine, CurrentState::TUTORIAL) {
	bool bUnitTest;

	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/cubeTex.png", "cubeTex"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/cubeTex_NormalMap.png", "cubeTexNormalMap"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/saturnTex.jpg", "saturn"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/saturn_rings.png", "saturnRings"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/crossHair.png", "crossHair"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/enemy01.jpg", "enemySphere"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/muzzleFlash.png", "muzzleFlash"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/Ammo.png", "ammo"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/HealthIcon.png", "health"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/SniperScope.png", "sniperScope"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/RedOrb.png", "redOrb"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/Camouflage.jpg", "clothTex"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/shockwave.png", "shockwave"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/drone.jpg", "drone"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/menu.png", "mainMenu"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/Indicator.png", "indicator"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/AboutBackground.png", "aboutMenu"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/PlayerDeathScreen.png", "playerDead"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/VictoryScreen.png", "victorious"); assert(bUnitTest);

	// Terrain textures
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/soil01.jpg", "soil"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/soil02.jpg", "soil2"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/soil03.jpg", "grass"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/soil03_NormalMap.jpg", "grassNormalMap"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/soil04.jpg", "soil4"); assert(bUnitTest);
	bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile("res/Textures/blendMap.png", "blendMap"); assert(bUnitTest);

	std::vector<char*> skyboxFaces =
	{
		"res/Textures/Skyboxes/TitanMoon/right.png", "res/Textures/Skyboxes/TitanMoon/left.png",
		"res/Textures/Skyboxes/TitanMoon/top.png", "res/Textures/Skyboxes/TitanMoon/bottom.png",
		"res/Textures/Skyboxes/TitanMoon/front.png", "res/Textures/Skyboxes/TitanMoon/back.png"
	};

	std::vector<char*> skyboxIDs = { "skybox", "skybox1", "skybox2", "skybox3", "skybox4", "skybox5" };

	for (auto i = 0; i < 6; ++i) {
		bUnitTest = ResourceManager::GetInstance().LoadTextureImagesFromFile(skyboxFaces.at(i), skyboxIDs.at(i));
		assert(bUnitTest);
	}

	ResourceManager::GetInstance().SetSkyboxTextureIDs();

	int id = 0;
	std::vector<char*> defShader{ "res/Shaders/DefaultVertexShader.vs", "res/Shaders/DefaultFragmentShader.fs" };
	std::vector<char*> unlitShader{ "res/Shaders/Unlit.vs", "res/Shaders/Unlit.fs" };
	std::vector<char*> skyboxShader{ "res/Shaders/SkyboxVertexShader.vs", "res/Shaders/SkyboxFragmentShader.fs" };
	std::vector<char*> normalMappingShader{ "res/Shaders/NormalMapping.vs", "res/Shaders/NormalMapping.fs" };
	std::vector<char*> hudShader{ "res/Shaders/HUD.vs", "res/Shaders/HUD.fs" };
	std::vector<char*> muzzleFlashShader{ "res/Shaders/Muzzle Flash Shader/VertexShaderMuzzleFlash.vs", "res/Shaders/Muzzle Flash Shader/FragmentShaderMuzzleFlash.fs" };
	std::vector<char*> enemyShader{ "res/Shaders/EnemyVertexShader.vs", "res/Shaders/EnemyFragmentShader.fs" };
	std::vector<char*> postProcessingShader{ "res/Shaders/PostProcessingVertexShader.vs", "res/Shaders/PostProcessingFragmentShader.fs" };

	// Initialize game models
	m_terrain.InitTerrain("res/Shaders/TerrainVertexShader.vs", "res/Shaders/TerrainFragmentShader.fs");
	m_terrain.CreateTerrainWithPerlinNoise();

	m_dirLight.Configure(glm::vec3(-0.1f, -0.1f, -0.1f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.5f, 0.5f, 0.5f));
	m_dirLight.SetDirection(glm::vec3(0.2f, 1.0f, 0.5f));
	m_dirLight.SetColour(glm::vec3(0.97f, 0.88f, 0.70f));

	m_pointLight.Configure(glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.045f, 0.0075f);
	m_pointLight.SetPosition(glm::vec3(256.0f, 50.0f, 300.0f));
	m_pointLight.SetLightColour(glm::vec3(0.0f, 0.0f, 1.0f));

	Player::GetInstance().Init();
}

Tutorial::~Tutorial() {

}

void Tutorial::fixedUpdate() {

}

void Tutorial::update() {
	Player::GetInstance().Update(m_terrain, m_dt);

	// Update physics component
	//Physics::GetInstance().Update(m_cameraVo, m_dt);
};

void Tutorial::render() {
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	Player::GetInstance().Animate(m_dt);

	RenderScene();

	if (Globals::drawUi)
		renderUi();
}

void Tutorial::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Tutorial::OnMouseButtonDown(Event::MouseButtonEvent& event) {

}

void Tutorial::OnMouseButtonUp(Event::MouseButtonEvent& event) {

}

void Tutorial::resize(int deltaW, int deltaH) {

}

void Tutorial::renderUi() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("InvisibleWindow", nullptr, windowFlags);
	ImGui::PopStyleVar(3);

	ImGuiID dockSpaceId = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::End();

	if (m_initUi) {
		m_initUi = false;
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Left, 0.2f, nullptr, &dockSpaceId);
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Tutorial::RenderScene() {
	// Draw flag (cloth simulation)
	//m_flag.AddForce(glm::vec3(0.0f, -0.2f, 0.0f) * 0.25f);
	//m_flag.WindForce(glm::vec3(0.7f, 0.1f, 0.2f) * 0.25f);
	//m_flag.Update();
	//m_flag.Draw(m_camera);
	const Camera& camera = Player::GetInstance().getCamera();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Draw flag pole 
	//m_flagPole.Draw(m_camera, glm::vec3(256.0f, m_terrain.GetHeightOfTerrain(256.0f, 300.0f) + 10.0f, 270.0f));

	// Draw big rocks around the corners
	/*for (auto i : m_mountainRocks)
		i.Draw(m_camera, false);

	// Check if the player is aiming
	if (Player::GetInstance().IsPlayerAiming())
	{
		// Render sniper scope
		Renderer::GetInstance().GetComponent(SNIPER_SCOPE).Draw(m_cameraHUD);

		// Zoom in and slow player speed and mouse sensitivity
		if (m_camera.GetCameraFOV() != 30.0f)
		{
			m_camera.InitCameraPerspective(30.0f, 1440.0f / 900.0f, 0.1f, 5000.0f);
			m_camera.SetCameraSpeed(7.0f);
			m_camera.SetCameraSensitivity(4.0f);
		}
	}
	else
	{
		// Render FPS crosshair if player is not using sniper rifle
		if (!Player::GetInstance().IsUsingSniper())
			Renderer::GetInstance().GetComponent(FPS_CROSSHAIR).Draw(m_cameraHUD);

		if (m_camera.GetCameraFOV() != 80.0f)
		{
			m_camera.InitCameraPerspective(80.0f, 1440.0f / 900.0f, 0.1f, 5000.0f);
			m_camera.SetCameraSpeed(20.0f);
			m_camera.SetCameraSensitivity(7.0f);
		}
	}

	Renderer::GetInstance().GetComponent(AMMO).Draw(m_cameraHUD);
	Renderer::GetInstance().GetComponent(HEALTH).Draw(m_cameraHUD);
	Renderer::GetInstance().GetComponent(SATURN_RINGS).Draw(m_camera);
	Renderer::GetInstance().GetComponent(SATURN).GetTransformComponent().GetRot().y += 2.0f * m_deltaTime;
	Renderer::GetInstance().GetComponent(SATURN).Draw(m_camera, glm::vec3(-700.0f, 700.0f, 0.0f));

	m_asteroid.DrawInstanced(m_camera);*/

	glDisable(GL_CULL_FACE);

	m_terrain.SetFog(m_atmosphere.GetDayTime() <= 0.3f ? false : true);
	m_terrain.Draw(camera, &m_dirLight, &m_pointLight, Player::GetInstance().GetSpotLight());

	/*Renderer::GetInstance().GetComponent(SKYBOX).GetTransformComponent().GetRot().y += 0.5f * m_deltaTime;
	Renderer::GetInstance().GetComponent(SKYBOX).Draw(m_camera);

	// Draw enemy units
	int enemyId = 100;
	for (unsigned int i = 0; i < m_enemyCount; ++i)
	{
		// Check if this enemy unit can respawn (if the data transfer is at 100, then this enemy cannot respawn anymore)
		m_enemies.at(i)->SetRespawnStatus(m_dataTransmitTimer < 100 ? true : false);
		m_enemies.at(i)->Draw(enemyId, ENEMY_DRONE);
		++enemyId;
	}

	// Draw enemy shockwave if smart drones have exploded
	for (unsigned int i = 0; i < m_enemyCount; ++i)
	{
		m_enemies.at(i)->DrawShockwave(ENEMY_BLAST);
	}

	// Text updates
	// [0] : Ammo, [1] : Health, [2] : Data transmission percentage
	m_texts[0].SetText(std::to_string(Player::GetInstance().GetCurrWeapon().GetAmmoCount()));
	m_texts[0].Render();

	m_texts[1].SetText(std::to_string(Player::GetInstance().GetHealth()));
	m_texts[1].Render();

	if (m_dataTransmitTimer < 100)
	{
		m_texts[2].SetText("Data transfer: " + std::to_string((int)m_dataTransmitTimer) + "%");
	}
	else
	{
		m_texts[2].SetSpacing(0.7f);
		m_texts[2].SetText("Data transferred! Defeat all remaining foes");
	}

	m_texts[2].Render();*/
}