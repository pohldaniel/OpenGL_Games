#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Tutorial.h"
#include "Constants.h"
#include "Physics.h"
#include "Player.h"
#include "Application.h"
#include "engine/Fontrenderer.h"

Tutorial::Tutorial(StateMachine& machine) : State(machine, CurrentState::TUTORIAL) {
	
	Player::GetInstance().Init();

	m_terrain.init();
	m_terrain.createTerrainWithPerlinNoise();

	m_dirLight.configure(Vector3f(-0.1f, -0.1f, -0.1f), Vector3f(0.1f, 0.1f, 0.1f), Vector3f(0.5f, 0.5f, 0.5f));
	m_dirLight.setDirection(Vector3f(0.2f, 1.0f, 0.5f));
	m_dirLight.setColour(Vector3f(0.97f, 0.88f, 0.70f));

	m_pointLight.configure(Vector3f(0.05f, 0.05f, 0.05f), Vector3f(10.0f, 10.0f, 10.0f), Vector3f(1.0f, 1.0f, 1.0f), 1.0f, 0.045f, 0.0075f);
	m_pointLight.setPosition(Vector3f(256.0f, 50.0f, 300.0f));
	m_pointLight.setColour(Vector3f(0.0f, 0.0f, 1.0f));

	m_skybox = RenderableObject("cube", "skybox", "titan");
	m_skybox.setScale(750.0f);
	m_skybox.setDrawFunction([&](const Camera& camera) {
		if (m_skybox.isDisabled()) return;

		glDisable(GL_DEPTH_TEST);
		glFrontFace(GL_CW);
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
		glFrontFace(GL_CCW);
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

	m_flagPoleModel.loadModel("res/Models3D/FlagPole/Pole.obj");
	m_flagPole = RenderableObject("weapon");
	m_flagPole.setDrawFunction([&](const Camera& camera) {
		if (m_flagPole.isDisabled()) return;

		auto shader = Globals::shaderManager.getAssetPointer(m_flagPole.getShader());

		shader->use();

		shader->loadVector("lightPos", Vector3f(camera.getPosition()[0], camera.getPosition()[1] + 5.0f, camera.getPosition()[2]));
		shader->loadVector("viewPos", camera.getPosition());
		shader->loadBool("EnableSpotlight", false);

		shader->loadMatrix("model", Matrix4f::Translate(256.0f, m_terrain.getHeightOfTerrain(256.0f, 300.0f) + 10.0f, 270.0f));
		shader->loadMatrix("projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("view", camera.getViewMatrix());
		shader->loadInt("texture_diffuse1", 0);
		m_flagPoleModel.getMesh()->getMaterial().bind(1);
		m_flagPoleModel.drawRaw();
		shader->unuse();
	});

	m_rockModel.loadModel("res/Models3D/Rock/LowPolyRock.dae");
	m_rockModel.getMesh()->getMaterial().getTexture(0).setLinear();
	m_rockModel.getMesh()->getMaterial().getTexture(0).setWrapMode();

	m_rockModel.addInstance(Matrix4f::Translate(30.0f, 60.0f, 15.0f) * Matrix4f::Rotate(0.0f, 180.0f, 0.0f) * Matrix4f::Scale(20.0f, 36.0f, 20.0f));
	m_rockModel.addInstance(Matrix4f::Translate(512.0f, 63.0f, 15.0f) * Matrix4f::Rotate(0.0f, 180.0f, 0.0f) * Matrix4f::Scale(20.0f, 36.0f, 20.0f));
	m_rockModel.addInstance(Matrix4f::Translate(750.0f, 63.0f, 15.0f) * Matrix4f::Rotate(0.0f, 100.0f, 0.0f) * Matrix4f::Scale(20.0f, 36.0f, 20.0f));
	m_rockModel.addInstance(Matrix4f::Translate(30.0f, 60.0f, 750.0f) * Matrix4f::Rotate(0.0f, 180.0f, 0.0f) * Matrix4f::Scale(20.0f, 36.0f, 20.0f));
	m_rockModel.addInstance(Matrix4f::Translate(750.0f, 63.0f, 750.0f) * Matrix4f::Rotate(0.0f, 100.0f, 0.0f) * Matrix4f::Scale(20.0f, 36.0f, 20.0f));

	std::vector<Matrix4f> modelMTXs;

	unsigned int amount = 20000;
	float radius = 400.0f;
	float offset = 60.0f;

	for (unsigned int i = 0; i < amount; ++i){
		
		float x = 0.0f, y = 0.0f, z = 0.0f;

		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset + 300.0f;
		x = sin(angle) * radius + displacement;

		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset + 40.0f;
		y = displacement * 3.7f;

		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset + 390.0f;
		z = cos(angle) * radius + displacement;

		float scale = (rand() % 25) / 100.0f + 0.35f;
		float rotAngle = (rand() % 360);
		modelMTXs.push_back(Matrix4f::Translate(x, y, z) * Matrix4f::Rotate(Vector3f(0.5f, 0.7f, 0.9f), rotAngle) * Matrix4f::Scale(scale, scale, scale));
	}
	m_rockModel.addInstances(modelMTXs);

	m_rock = RenderableObject("instance");
	m_rock.setDrawFunction([&](const Camera& camera) {
		if (m_rock.isDisabled()) return;

		auto shader = Globals::shaderManager.getAssetPointer(m_rock.getShader());

		glUseProgram(shader->m_program);

		shader->loadMatrix("projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("view", camera.getViewMatrix());

		m_rockModel.getMesh()->getMaterial().getTexture(0).bind();
		m_rockModel.drawRawInstanced();
		glUseProgram(0);
	});

	m_sniperScope = RenderableObject("quad", "hud", "sniperScope");

	m_sniperScope.setDrawFunction([&](const Camera& camera) {
		if (m_sniperScope.isDisabled()) return;

		auto shader = Globals::shaderManager.getAssetPointer(m_sniperScope.getShader());		
		shader->use();
		shader->loadMatrix("projection", camera.getOrthographicMatrix());
		shader->loadMatrix("model", Matrix4f::Scale(static_cast<float>(Application::Width / 2), static_cast<float>(Application::Height / 2), 1.0f));
		Globals::textureManager.get(m_sniperScope.getTexture()).bind(0);
		Globals::shapeManager.get(m_sniperScope.getShape()).drawRaw();
		shader->unuse();
	});

	m_crossHaire = RenderableObject("quad", "hud", "crossHair");
	m_crossHaire.setDrawFunction([&](const Camera& camera) {
		if (m_crossHaire.isDisabled()) return;

		auto shader = Globals::shaderManager.getAssetPointer(m_crossHaire.getShader());
		shader->use();
		shader->loadMatrix("projection", Matrix4f::IDENTITY);
		shader->loadMatrix("model", Matrix4f::Scale(0.05f, 0.05f, 1.0f));
		Globals::textureManager.get(m_crossHaire.getTexture()).bind(0);
		Globals::shapeManager.get(m_crossHaire.getShape()).drawRaw();
		shader->unuse();
	});

	m_health = RenderableObject("quad", "hud", "health");
	m_health.setPosition(-0.9f, -0.9f, 0.0f);
	m_health.setScale(0.0475f, 0.06f, 1.0f);

	m_health.setDrawFunction([&](const Camera& camera) {
		if (m_health.isDisabled()) return;

		auto shader = Globals::shaderManager.getAssetPointer(m_health.getShader());
		shader->use();
		shader->loadMatrix("projection", Matrix4f::IDENTITY);
		shader->loadMatrix("model", m_health.getTransformationSP());
		Globals::textureManager.get(m_health.getTexture()).bind(0);
		Globals::shapeManager.get(m_health.getShape()).drawRaw();
		shader->unuse();
	});

	m_ammo = RenderableObject("quad", "hud", "ammo");
	m_ammo.setPosition(-0.7f, -0.9f, 0.0f);
	m_ammo.setOrientation(0.0f, 0.0f, 15.0f);
	m_ammo.setScale(0.035f, 0.035f, 1.0f);

	m_ammo.setDrawFunction([&](const Camera& camera) {
		if (m_health.isDisabled()) return;

		auto shader = Globals::shaderManager.getAssetPointer(m_ammo.getShader());
		shader->use();
		shader->loadMatrix("projection", Matrix4f::IDENTITY);
		shader->loadMatrix("model", m_ammo.getTransformationSOP());
		Globals::textureManager.get(m_ammo.getTexture()).bind(0);
		Globals::shapeManager.get(m_ammo.getShape()).drawRaw();
		shader->unuse();
	});
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
	// Update data transmitter 
	m_dataTransmitTimer += 0.59f * m_dt;
	// Update physics component
	//Physics::GetInstance().Update(m_cameraVo, m_dt);
};

void Tutorial::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	const Camera& camera = Player::GetInstance().getCamera();
	m_skybox.draw(camera);
	m_saturn.draw(camera);
	m_saturnRings.draw(camera);
	m_flagPole.draw(camera);
	m_flag.Draw(camera);
	m_rock.draw(camera);

	Player::GetInstance().Animate(m_dt);


	m_terrain.setFog(m_atmosphere.GetDayTime() <= 0.3f ? false : true);
	m_terrain.draw(camera, &m_dirLight, &m_pointLight, Player::GetInstance().GetSpotLight());
	
	if (Player::GetInstance().IsPlayerAiming()){
		glDepthFunc(GL_ALWAYS);
		m_sniperScope.draw(camera);
		glDepthFunc(GL_LESS);	
	}else
		m_crossHaire.draw(camera);
		
	m_health.draw(camera);
	m_ammo.draw(camera);

	Globals::spritesheetManager.getAssetPointer("font")->bind(0);
	Fontrenderer::Get().addText(Globals::fontManager.get("roboto_28"), 80, 20, std::to_string(Player::GetInstance().GetHealth()), Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("roboto_28"), 180, 20, std::to_string(Player::GetInstance().GetCurrWeapon().getAmmoCount()), Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("roboto_20"), 380, 25, "Data transfer: " + Fontrenderer::FloatToString(m_dataTransmitTimer, 1) + "%", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Fontrenderer::Get().drawBuffer();

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