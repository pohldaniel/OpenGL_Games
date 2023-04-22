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
	m_terrain.init();
	m_terrain.createTerrainWithPerlinNoise();

	m_dirLight.configure(Vector3f(-0.1f, -0.1f, -0.1f), Vector3f(0.1f, 0.1f, 0.1f), Vector3f(0.5f, 0.5f, 0.5f));
	m_dirLight.setDirection(Vector3f(0.2f, 1.0f, 0.5f));
	m_dirLight.setColour(Vector3f(0.97f, 0.88f, 0.70f));

	m_pointLight.configure(Vector3f(0.05f, 0.05f, 0.05f), Vector3f(10.0f, 10.0f, 10.0f), Vector3f(1.0f, 1.0f, 1.0f), 1.0f, 0.045f, 0.0075f);
	m_pointLight.setPosition(Vector3f(256.0f, 50.0f, 300.0f));
	m_pointLight.setColour(Vector3f(0.0f, 0.0f, 1.0f));

	Player::GetInstance().Init();
	m_cube = new Cube();

	m_skybox = RenderableObject("cube", "skybox", "titan");
	m_skybox.setScale(750.0f);
	m_skybox.setDrawFunction([&](const Camera& camera, bool viewIndependent) {
		if (m_skybox.isDisabled()) return;

		glDisable(GL_DEPTH_TEST);
		auto shader = Globals::shaderManager.getAssetPointer(m_skybox.getShader());
		Matrix4f view = camera.getViewMatrix();
		view[3][0] = 0.0f; view[3][1] = 0.0f; view[3][2] = 0.0f;
		shader->use();
		shader->loadMatrix("projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("view", view);
		shader->loadMatrix("model", m_skybox.getTransformationSO());
		shader->loadVector("lightPos", Vector3f(0.0f, 0.0f, 0.0f));
		shader->loadVector("viewPos", camera.getPosition());
		shader->loadInt("cubemap", 0);

		Globals::textureManager.get(m_skybox.getTexture()).bind(0);
		Globals::shapeManager.get(m_skybox.getShape()).drawRaw();

		Texture::Unbind(GL_TEXTURE_CUBE_MAP);

		shader->unuse();
		glEnable(GL_DEPTH_TEST);
	});	

	m_skybox.setUpdateFunction(
		[&](const float dt) {
		m_skybox.rotate(0.0f, 10.5f * PI_ON_180 * m_dt, 0.0f);
	});

	m_saturn = RenderableObject("sphere", "default", "saturn");
	m_saturn.setPosition(200.0f, 350.0f, -700.0f);
	m_saturn.setScale(95.0f, 95.0f, 95.0f);
	m_saturn.setOrientation(Vector3f(25.0f, 90.0f, 0.0f));

	m_saturn.setUpdateFunction(
		[&](const float dt) {
		m_saturn.rotate(0.0f, 20.0f * m_dt, 0.0f);
	});

	m_saturnRings = RenderableObject("quad", "default", "saturnRings");
	m_saturnRings.setPosition(200.0f, 360.0f, -700.0f);
	m_saturnRings.setOrientation(Vector3f(-65.0f, 0.0f, 0.0f));
	m_saturnRings.setScale(640.0f, 640.0f, 640.0f);
	
	m_flag.Configure(10, 6, 20, 17);
	m_flag.SetPos(Vector3f(256.4f, m_terrain.getHeightOfTerrain(256.0f, 300.0f) + 45.0f, 270.0f));

	m_flagPole.loadModel("res/Models3D/FlagPole/Pole.obj");
}

Tutorial::~Tutorial() {

}

void Tutorial::fixedUpdate() {

}

void Tutorial::update() {
	Keyboard &keyboard = Keyboard::instance();
	if (keyboard.keyPressed(Keyboard::KEY_F)) {
		m_skybox.setDisabled(!m_skybox.isDisabled());
	}

	m_skybox.update(m_dt);
	m_saturn.update(m_dt);

	m_flag.AddForce(Vector3f(0.0f, -0.2f, 0.0f) * 0.25f);
	m_flag.WindForce(Vector3f(0.7f, 0.1f, 0.2f) * 0.25f);
	m_flag.Update();

	Player::GetInstance().Update(m_terrain, m_dt);

	// Update physics component
	//Physics::GetInstance().Update(m_cameraVo, m_dt);
};

void Tutorial::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	const Camera& camera = Player::GetInstance().getCamera();
	m_skybox.draw(camera);
	m_saturn.draw(camera);
	m_saturnRings.draw(camera);
	auto shader = Globals::shaderManager.getAssetPointer("weapon");
	shader->use();

	shader->use();
	shader->loadMatrix("model", Matrix4f::Translate(256.0f, m_terrain.getHeightOfTerrain(256.0f, 300.0f) + 10.0f, 270.0f));
	shader->loadVector("lightPos", Vector3f(camera.getPosition()[0], camera.getPosition()[1] + 5.0f, camera.getPosition()[2]));
	shader->loadVector("viewPos", camera.getPosition());
	shader->loadBool("EnableSpotlight", false);

	
	shader->loadMatrix("projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("view", camera.getViewMatrix());
	shader->loadInt("texture_diffuse1", 0);
	Globals::textureManager.get("flagPole").bind(0);

	m_flagPole.drawRaw();
	shader->unuse();

	m_flag.Draw(camera);
	Player::GetInstance().Animate(m_dt);

	glDisable(GL_CULL_FACE);

	m_terrain.setFog(m_atmosphere.GetDayTime() <= 0.3f ? false : true);
	m_terrain.draw(camera, &m_dirLight, &m_pointLight, Player::GetInstance().GetSpotLight());

	//RenderScene();

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

	m_terrain.setFog(m_atmosphere.GetDayTime() <= 0.3f ? false : true);
	m_terrain.draw(camera, &m_dirLight, &m_pointLight, Player::GetInstance().GetSpotLight());

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