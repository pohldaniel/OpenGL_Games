#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Game.h"
#include "Application.h"
#include "Constants.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), 
									m_keySet(m_player.getPosition()), 
									m_respawnPointSet(m_player.getPosition()),
									m_columnSet(m_player.getPosition()),
									m_player(m_camera) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow());

	m_terrain.init("Levels/terrain01.raw");
	m_quadTree.init(m_terrain.getPositions().data(), m_terrain.getIndexBuffer().data(), m_terrain.getIndexBuffer().size(), m_terrain.getMin(), m_terrain.getMax(), 64.0f);

	Init();

	const Vector3f& pos = Vector3f((TERRAIN_SIZE * SCALE) / 2, (m_terrain.heightAt((TERRAIN_SIZE * SCALE) / 2, (TERRAIN_SIZE * SCALE) / 2) + RADIUS) * SCALE, (TERRAIN_SIZE * SCALE) / 2);
	
	m_camera = ThirdPersonCamera();
	m_camera.perspective(45.0f, (float)Application::Width / (float)Application::Height, 0.1f, 5000.0f);
	m_camera.lookAt(pos - Vector3f(0.0f, 0.0f, m_offsetDistance), pos, Vector3f(0.0f, 1.0f, 0.0f));

	std::vector<btCollisionShape*> terrainShape = Physics::CreateStaticCollisionShapes(&m_terrain, SCALE);
	btRigidBody* body = Globals::physics->addStaticModel(terrainShape, Physics::BtTransform(), false, btVector3(1.0f, 1.0f, 1.0f), Physics::collisiontypes::TERRAIN, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::CAMERA);
}

Game::~Game() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Game::fixedUpdate() {
	m_player.getCharacterController()->preStep();
	Globals::physics->stepSimulation(m_fdt);
	m_player.getCharacterController()->postStep();
}

void Game::update() {
	
	m_player.update(m_dt);
	const Vector3f& playerPos = m_player.getPosition();
	
	if (pickedkey_id == -1){
		for (unsigned int i = 0; i < m_keySet.getKeyStates().size(); i++){

			if (!m_keySet.isDeployed(i)){
				if ((playerPos - m_keySet.getPosition(i)).length() <= RADIUS * 2){
					pickedkey_id = i;					
					m_keySet.setPickedKeyId(pickedkey_id);
					break;
					//Sound.Play(SOUND_PICKUP);
				}
			}
		}

	}else {
		if (m_columnSet.insideGatheringArea(pickedkey_id)) {
			//Sound.Play(SOUND_UNLOCK);
			//Sound.Play(SOUND_ENERGYFLOW);

			const Vector3f& columnPos = m_columnSet.getPosition(pickedkey_id);
			m_keySet.deploy(pickedkey_id, m_columnSet.getHole(pickedkey_id), m_columnSet.getYaw(pickedkey_id));
			m_raySet.deploy(Vector3f(columnPos[0], columnPos[1] + COLUMN_HEIGHT + ENERGY_BALL_RADIUS, columnPos[2]), m_portal.getReceptor(pickedkey_id), pickedkey_id, m_keySet.getNumDeployed());
			
			pickedkey_id = -1;
			if (respawn_id) {
				//Sound.Play(SOUND_SWISH);
				respawn_id = 0;
			}
			portal_activated = m_keySet.getNumDeployed() == 5;
			m_portal.setDisabled(!portal_activated);
			//if (portal_activated) Sound.Play(SOUND_WARP);
		}
	}

	m_portal.update(m_dt);
	m_keySet.update(m_dt);
	m_raySet.update(m_dt);
	m_respawnPointSet.update(m_dt);
}

void Game::render() {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&m_camera.getPerspectiveMatrix()[0][0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(&m_camera.getViewMatrix()[0][0]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_skybox.draw(m_camera);
	

	auto shader = Globals::shaderManager.getAssetPointer("terrain_new");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", Matrix4f::Scale(SCALE, SCALE, SCALE));
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix()));
	
	shader->loadVector("lightPos", Vector3f(50.0f, 50.0f, 50.0f));
	shader->loadVector("lightAmbient", Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	shader->loadVector("lightDiffuse", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	shader->loadVector("lightSpecular", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

	shader->loadVector("matAmbient", Vector4f(0.7f, 0.7f, 0.7f, 1.0f));
	shader->loadVector("matDiffuse", Vector4f(0.8f, 0.8f, 0.8f, 1.0f));
	shader->loadVector("matSpecular", Vector4f(0.3f, 0.3f, 0.3f, 1.0f));
	shader->loadVector("matEmission", Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	shader->loadFloat("matShininess", 100.0f);

	shader->loadInt("tex_top", 0);
	shader->loadInt("tex_side", 1);
	shader->loadFloat("height", Lava.GetHeight());
	shader->loadFloat("hmax", Lava.GetHeightMax());

	Globals::textureManager.get("grass").bind(0);
	Globals::textureManager.get("rock").bind(1);
	QuadTree::Frustum.updatePlane(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
	int TrianglesRendered = m_quadTree.checkVisibility();

	m_terrain.bindVAO();
	m_quadTree.draw();
	m_terrain.unbindVAO();
	shader->unuse();

	m_keySet.draw(m_camera);
	m_raySet.draw(m_camera);
	m_columnSet.draw(m_camera);
	
	if (abs(m_camera.getPositionZ() - m_portal.getZ()) < m_camera.getOffsetDistance()){
		//draw player
		m_player.draw(m_camera);

		//draw portal
		m_portal.draw(m_camera);
	}else{
		//draw portal
		m_portal.draw(m_camera);

		//draw player
		m_player.draw(m_camera);
	}

	m_respawnPointSet.draw(m_camera);
	m_lava.draw(m_camera);

	if (m_drawUi)
		renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Game::OnMouseWheel(Event::MouseWheelEvent& event) {

	if (event.direction == 1u) {
		m_offsetDistance += 2.0f;
		m_offsetDistance = std::max(0.0f, std::min(m_offsetDistance, 150.0f));
		m_camera.setOffsetDistance(m_offsetDistance);
	}

	if (event.direction == 0u) {
		m_offsetDistance -= 2.0f;
		m_offsetDistance = std::max(0.0f, std::min(m_offsetDistance, 150.0f));
		m_camera.setOffsetDistance(m_offsetDistance);
	}
}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		m_drawUi = false;
		Mouse::instance().attach(Application::GetWindow());
		Keyboard::instance().enable();
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {

}

void Game::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_SPACE) {
		m_drawUi = true;
		Mouse::instance().detach();
		Keyboard::instance().disable();
	}
}

void Game::resize(int deltaW, int deltaH) {

}

void Game::renderUi() {
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

	if (ImGui::SliderFloat("Camera Offset", &m_offsetDistance, 0.0f, 150.0f)) {
		m_camera.setOffsetDistance(m_offsetDistance);
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::Init() {

	bool res = true;
	portal_activated = false;
	respawn_id = 0;
	pickedkey_id = -1;
	ang = 0.0f;

	//_Terrain.Load(1);
	//_Terrain.createAttribute();
	Lava.Load(TERRAIN_SIZE);

	//Sound.Play(SOUND_AMBIENT);
	m_player.init(m_terrain);
	m_keySet.init(m_terrain);
	m_respawnPointSet.init(m_terrain);
	m_columnSet.init(m_terrain);
	m_portal.init(m_terrain);
	m_raySet.init();

	m_lava = RenderableObject("quad_lava", "texture_new", "lava");
	m_lava.setPosition(0.0f, 2.5f, 0.0f);
	m_lava.setDrawFunction([&](const Camera& camera) {
		if (m_lava.isDisabled()) return;

		auto shader = Globals::shaderManager.getAssetPointer(m_lava.getShader());
		shader->use();
		shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", camera.getViewMatrix());
		shader->loadMatrix("u_model", m_lava.getTransformationP());
		shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix() * m_lava.getTransformationP()));
		Globals::textureManager.get(m_lava.getTexture()).bind(0);
		Globals::shapeManager.get(m_lava.getShape()).drawRaw();
		shader->unuse();
	});

	m_skybox = RenderableObject("cube", "skybox", "skybox");
	m_skybox.setDrawFunction([&](const Camera& camera) {
		if (m_skybox.isDisabled()) return;
		glDepthFunc(GL_LEQUAL);
		glFrontFace(GL_CW);

		auto shader = Globals::shaderManager.getAssetPointer(m_skybox.getShader());
		Matrix4f view = camera.getViewMatrix();
		view[3][0] = 0.0f; view[3][1] = 0.0f; view[3][2] = 0.0f;
		shader->use();
		shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", view);
		shader->loadInt("u_texture", 0);

		Globals::textureManager.get(m_skybox.getTexture()).bind(0);
		Globals::shapeManager.get(m_skybox.getShape()).drawRaw();

		Texture::Unbind(GL_TEXTURE_CUBE_MAP);

		shader->unuse();

		glFrontFace(GL_CCW);
		glDepthFunc(GL_LESS);
	});

}