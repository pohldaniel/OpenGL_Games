#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Game.h"
#include "Application.h"
#include "Menu.h"
#include "Globals.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), 
									m_keySet(m_player.getPosition()), 
									m_respawnPointSet(m_player.getPosition()),
									m_columnSet(m_player.getPosition()),
									m_player(m_camera, m_lava),
									m_lavaTriggerResult(m_keySet, m_respawnPointSet),
									m_portalTriggerResult(*this),
									pickedKeyId(m_keySet.getPickedKeyId()),
									m_cloudsModel(Application::Width, Application::Height, m_light),
									m_sky(Application::Width, Application::Height, m_light) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow());

	m_terrain.init("res/terrain01.raw");
	m_quadTree.init(m_terrain.getPositions().data(), m_terrain.getIndexBuffer().data(), static_cast<unsigned int>(m_terrain.getIndexBuffer().size()), m_terrain.getMin(), m_terrain.getMax(), 64.0f);

	const Vector3f& pos = Vector3f(512.0f, (m_terrain.heightAt(512.0f, 512.0f) + RADIUS), 512.0f);
	init();

	m_camera = ThirdPersonCamera();
	m_camera.perspective(45.0f, (float)Application::Width / (float)Application::Height, 0.1f, 1500.0f);
	m_camera.lookAt(pos - Vector3f(0.0f, 0.0f, m_offsetDistance), pos, Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setOffsetDistance(m_offsetDistance);

	std::vector<btCollisionShape*> terrainShape = Physics::CreateStaticCollisionShapes(&m_terrain, 1.0f);
	btRigidBody* body = Globals::physics->addStaticModel(terrainShape, Physics::BtTransform(), false, btVector3(1.0f, 1.0f, 1.0f), Physics::collisiontypes::TERRAIN, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::CAMERA);

	m_light.color = Vector3f(255.0f, 255.0f, 230.0f) / 255.0f;
	m_light.direction = Vector3f(-.5f, 0.5f, 1.0f);
	m_light.position = m_light.direction*1e6f + m_camera.getPosition();

	m_fogColor = m_sky.getFogColor();

	sceneBuffer.create(Application::Width, Application::Height);
	sceneBuffer.attachTexture2D(AttachmentTex::RGBA32F);
	sceneBuffer.attachTexture2D(AttachmentTex::DEPTH24);

	//half extents
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(512.0f, 0.0f, 512.0f));
	m_lava.create(new btBox2dShape(btVector3(512.0f, 0.0f, 512.0f)), transform, Physics::GetDynamicsWorld(), Physics::collisiontypes::TRIGGER | Physics::collisiontypes::LAVA, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::CAMERA);

	transform.setIdentity();
	transform.setOrigin(btVector3(512.0f, m_terrain.heightAt(512.0f, 512.0f + 32.0f) + 1.5f, 512.0f + 32.0f));
	m_portal.create(new btBoxShape(btVector3(1.5f, 1.5f, 0.01f)), transform, Physics::GetDynamicsWorld(), Physics::collisiontypes::TRIGGER, Physics::collisiontypes::CHARACTER);

	transform.setIdentity();
	m_ground.create(new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), 0.0f), transform, Physics::GetDynamicsWorld(), Physics::collisiontypes::TRIGGER, Physics::collisiontypes::CHARACTER);

	if (!Globals::musicManager.get("background").isPlaying())
		Globals::musicManager.get("background").play("res/sounds/ambient.mp3");
}

Game::~Game() {
	Globals::physics->removeAllCollisionObjects();
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Game::fixedUpdate() {

	m_player.getCharacterController()->preStep();
	Globals::physics->stepSimulation(m_fdt);
	m_player.getCharacterController()->postStep();
	m_player.fixedUpdate(m_fdt);

	Physics::GetDynamicsWorld()->contactPairTest(m_player.getContactObject(), m_lava.getCollisionObject(), m_lavaTriggerResult);
	Physics::GetDynamicsWorld()->contactPairTest(m_player.getContactObject(), m_ground.getCollisionObject(), m_lavaTriggerResult);
#if DEBUGCOLLISION
	Physics::GetDynamicsWorld()->contactPairTest(m_player.getContactObject(), m_portal.getCollisionObject(), m_portalTriggerResult);
#else
	if (m_portalActivated)
		Physics::GetDynamicsWorld()->contactPairTest(m_player.getContactObject(), m_portal.getCollisionObject(), m_portalTriggerResult);
#endif
}

void Game::update() {

	m_player.update(m_dt);
	const Vector3f& playerPos = m_player.getPosition();

	if (pickedKeyId == -1){
		for (unsigned int i = 0; i < m_keySet.getKeyStates().size(); i++){

			if (!m_keySet.isDeployed(i)){
				if ((playerPos - m_keySet.getPosition(i)).length() <= RADIUS * 2){
					m_keySet.pickKey(i);
					Globals::soundManager.get("game").playChannel(4u);
					break;
				}
			}
		}

	}else {
		if (m_columnSet.insideGatheringArea(pickedKeyId)) {
			Globals::soundManager.get("game").playChannel(6u);
			Globals::soundManager.get("game").playChannel(3u);

			const Vector3f& columnPos = m_columnSet.getPosition(pickedKeyId);
			m_keySet.deploy(m_columnSet.getHole(pickedKeyId), m_columnSet.getYaw(pickedKeyId));
			m_raySet.deploy(Vector3f(columnPos[0], columnPos[1] + COLUMN_HEIGHT + ENERGY_BALL_RADIUS, columnPos[2]), m_portal.getReceptor(pickedKeyId), pickedKeyId, m_keySet.getNumDeployed());
			m_respawnPointSet.deploy();
			pickedKeyId = -1;

			
			m_portalActivated = m_keySet.getNumDeployed() == 5;
			m_portal.setDisabled(!m_portalActivated);
			if (m_portalActivated)
				Globals::soundManager.get("game").playChannel(7u);
		}
	}

	m_portal.update(m_dt);
	m_keySet.update(m_dt);
	m_raySet.update(m_dt);
	m_respawnPointSet.update(m_dt);

	m_light.update(m_camera.getPosition());
	m_sky.update();
	m_cloudsModel.update();

	Keyboard &keyboard = Keyboard::instance();
	if (keyboard.keyPressed(Keyboard::KEY_F1)) {
		Globals::soundManager.get("game").playChannel(2u);
		m_player.setPosition(m_respawnPointSet.getActivePoistion());
		m_player.resetOrientation();
		m_keySet.restorePrevState();
	}
}

void Game::render() {
	if (!m_useSkybox) {
		m_sky.draw(m_camera);
		m_cloudsModel.draw(m_camera, m_sky);
	}

	sceneBuffer.bind();
	glClearColor(m_fogColor[0], m_fogColor[1], m_fogColor[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto quad = Globals::shaderManager.getAssetPointer("quad_back");
	if (!m_useSkybox) {
		quad->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_cloudsModel.getPostTexture());
		Globals::shapeManager.get("quad").drawRaw();
		quad->unuse();
	}else {
		m_skybox.draw(m_camera);
	}

	glEnable(GL_DEPTH_TEST);
	auto shader = Globals::shaderManager.getAssetPointer("terrain");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", Matrix4f::IDENTITY);
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
	shader->loadFloat("height", m_lava.getHeight());
	shader->loadFloat("hmax", m_lava.getHeightMax());

	Globals::textureManager.get("grass").bind(0);
	Globals::textureManager.get("rock").bind(1);
	QuadTree::Frustum.updatePlane(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
	int TrianglesRendered = m_quadTree.checkVisibility();

	m_terrain.bindVAO();
	m_quadTree.draw();
	m_terrain.unbindVAO();
	shader->unuse();

	m_raySet.draw(m_camera);
	m_columnSet.draw(m_camera);
	m_lava.draw(m_camera);
	m_keySet.draw(m_camera);

	if (abs(m_camera.getPositionZ() - m_portal.getZ()) < m_camera.getOffsetDistance()) {
		m_player.draw(m_camera);
		m_portal.draw(m_camera);
	}else {
		m_portal.draw(m_camera);
		m_player.draw(m_camera);
	}

	m_respawnPointSet.draw(m_camera);
	
	sceneBuffer.unbind();

#if DEVBUILD
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	quad->use();
	sceneBuffer.bindColorTexture(0, 0);
	Globals::shapeManager.get("quad").drawRaw();
	quad->unuse();

#if DEVBUILD
	glPolygonMode(GL_FRONT_AND_BACK, StateMachine::GetEnableWireframe() ? GL_LINE : GL_FILL);
	if (m_drawUi)
		renderUi();
#endif
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
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = true;
		Mouse::instance().detach();
		Keyboard::instance().disable();
	}
#endif
	if (event.keyCode == VK_ESCAPE) {
		Globals::musicManager.get("background").stop();
		ImGui::GetIO().WantCaptureMouse = false;
		Mouse::instance().detach();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1500.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_cloudsModel.resize(Application::Width, Application::Height);
	m_sky.resize(Application::Width, Application::Height);
	sceneBuffer.resize(Application::Width, Application::Height);
}

void toggleButton(const char* str_id, bool* v) {
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float height = ImGui::GetFrameHeight();
	float width = height * 1.55f;
	float radius = height * 0.50f;

	ImGui::InvisibleButton(str_id, ImVec2(width, height));
	if (ImGui::IsItemClicked())
		*v = !*v;

	float t = *v ? 1.0f : 0.0f;

	ImGuiContext& g = *GImGui;
	float ANIM_SPEED = 0.08f;
	if (g.LastActiveId == g.CurrentWindow->GetID(str_id))// && g.LastActiveIdTimer < ANIM_SPEED)
	{
		float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
		t = *v ? (t_anim) : (1.0f - t_anim);
	}

	ImU32 col_bg;
	if (ImGui::IsItemHovered())
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), ImVec4(0.64f, 0.83f, 0.34f, 1.0f), t));
	else
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImVec4(0.56f, 0.83f, 0.26f, 1.0f), t));

	draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
	draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
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
	toggleButton("Use Skybox", &m_useSkybox);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::init() {

	m_portalActivated = false;
	m_player.init(m_terrain);
	m_keySet.init(m_terrain);
	m_respawnPointSet.init(m_terrain);
	m_columnSet.init(m_terrain);
	m_portal.init(m_terrain);
	m_raySet.init();

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

btScalar PortalTriggerCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {

#if DEBUGCOLLISION
	Player* player = reinterpret_cast<Player*>(colObj0Wrap->getCollisionObject()->getUserPointer());
	player->setColor(Vector4f(1.0f, 0.0, 0.0f, 1.0f));
#else
	Globals::musicManager.get("background").stop();
	ImGui::GetIO().WantCaptureMouse = false;
	Mouse::instance().detach();
	game.m_isRunning = false;
	game.m_machine.addStateAtBottom(new Menu(game.m_machine));
#endif
	return 0;
}

btScalar LavaTriggerCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
	Globals::soundManager.get("game").playChannel(2u);
	Player* player = reinterpret_cast<Player*>(colObj0Wrap->getCollisionObject()->getUserPointer());
	player->setPosition(respawnPointSet.getActivePoistion());
	player->resetOrientation();
	keySet.restorePrevState();
	return 0;
}