#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <engine/Batchrenderer.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"

Game::Game(StateMachine& machine) : State(machine, States::GAME) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	//Mouse::instance().attach(Application::GetWindow());

	m_camera = Camera();
	m_camera.perspective(45.0f * _180_ON_PI, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 120.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.5f, 0.0f), Vector3f(0.0f, 0.5f, 0.0f) - Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 1.0f, 0.0f));

	m_camera.setPosition(10.0f, 0.5f, 0.0f);
	m_camera.setRotationSpeed(1.0f);
	m_camera.setMovingSpeed(1.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	glGenBuffers(1, &Globals::lightUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, Globals::lightUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightStruct) * 20, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, Globals::lightBinding, Globals::lightUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	auto shader = Globals::shaderManager.getAssetPointer("antware");
	aw::Material::setUniformsLocation(shader->m_program);

	m_model.loadModel("res/models/Ant.glb", false, false, false);
	Material& material = Material::GetMaterials()[m_model.getMeshes()[0]->getMaterialIndex()];

	m_gun.loadModel("res/models/Gun.glb", false, false, false);
	Material::GetMaterials()[m_gun.getMeshes()[0]->getMaterialIndex()].textures[0].loadFromFile("res/textures/Gun.png", true);
	m_gun.initShader();

	m_ant = new Ant();
	m_ant->loadSequence("res/animations/ant_walkcycle");
	m_ant->addMesh(m_model.getMeshes()[0]->getVertexBuffer(), m_model.getMeshes()[0]->getIndexBuffer());
	m_ant->loadSequenceGpu();
	m_ant->setPosition(0.0f, 0.0f, -10.0f);
	m_ant->start();

	m_muzzleMesh = std::make_shared<aw::Mesh>("res/models/MuzzleQuad.glb", nullptr, false);
	m_bulletMesh = std::make_shared<aw::Mesh>("res/models/Bullet.glb", "res/textures/Bullet.png", false);
	m_gunMesh = std::make_shared<aw::Mesh>("res/models/Gun.glb", "res/textures/Gun.png", false);
	m_handsMesh = std::make_shared<aw::Mesh>("res/models/Hands.glb", "res/textures/Hands.png", false);
	m_glovesMesh = std::make_shared<aw::Mesh>("res/models/Gloves.glb", "res/textures/Gloves.png", false);
	m_cpuMesh = std::make_shared<aw::Mesh>("res/models/CPU.glb", "res/textures/CPU.jpg", false);
	m_platformMesh = std::make_shared<aw::Mesh>("res/models/Platform.glb", nullptr, false);

	m_meshes.push_back(m_muzzleMesh);
	m_meshes.push_back(m_bulletMesh);
	m_meshes.push_back(m_gunMesh);
	m_meshes.push_back(m_handsMesh);
	m_meshes.push_back(m_glovesMesh);
	m_meshes.push_back(m_cpuMesh);
	m_meshes.push_back(m_platformMesh);
	aw::Mesh::constructVAO(m_meshes);

	m_player = new Player(m_camera,std::make_shared<aw::Mesh>("res/models/PlayerCube.glb"),
		aw::Material(),
		m_bulletMesh,
		glm::vec2(-51.5f, -51.5f),
		glm::vec2(51.5f, 51.5f),
		nullptr);
	m_player->transform = aw::Transform(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	m_muzzleGO = new aw::StaticGO(m_muzzleMesh, aw::Material(), m_player);
	m_gunGO = new aw::StaticGO(m_gunMesh, aw::Material(), m_player);
	m_handsGO = new aw::StaticGO(m_handsMesh, aw::Material(), m_player);
	m_glovesGO = new aw::StaticGO(m_glovesMesh, aw::Material(), m_player);
	m_cpuGO = new aw::StaticGO(m_cpuMesh, aw::Material(), nullptr);
	m_platformGO = new aw::StaticGO(m_platformMesh, aw::Material(), nullptr);

	m_player->start();
}

Game::~Game() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Game::fixedUpdate() {
	m_player->fixedUpdate(m_fdt);
}

void Game::update() {
	m_ant->update(m_dt);
	m_ant->rotate(0.0f, 30.0f * m_dt, 0.0f);
	m_player->update(m_dt);
}

void Game::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", m_ant->getTransformationMatrix());
	Globals::textureManager.get("ant").bind();
	m_ant->drawRaw();
	Globals::textureManager.get("ant").unbind();
	shader->unuse();

	//m_gun.draw(m_camera);
	
	shader = Globals::shaderManager.getAssetPointer("antware");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * Matrix4f::IDENTITY));
	//shader->loadMatrix("u_model", Matrix4f::IDENTITY);

	//m_bulletMesh->draw();
	//m_gunMesh->draw();
	//m_handsMesh->draw();
	//m_glovesMesh->draw();
	shader->loadMatrix("u_model", (const float*)glm::value_ptr(m_gunGO->applyTransform()));
	m_gunGO->draw();
	shader->loadMatrix("u_model", (const float*)glm::value_ptr(m_handsGO->applyTransform()));
	m_handsGO->draw();
	shader->loadMatrix("u_model", (const float*)glm::value_ptr(m_glovesGO->applyTransform()));
	m_glovesGO->draw();

	shader->loadMatrix("u_model", (const float*)glm::value_ptr(m_cpuGO->applyTransform()));
	m_cpuGO->draw();
	shader->loadMatrix("u_model", (const float*)glm::value_ptr(m_platformGO->applyTransform()));
	m_platformGO->draw();
	shader->loadMatrix("u_model", (const float*)glm::value_ptr(m_muzzleGO->applyTransform()));
	m_muzzleGO->draw();

	//m_player->draw();

	shader->unuse();

	if (m_drawUi)
		renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void Game::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Game::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void Game::OnKeyUp(Event::KeyboardEvent& event) {

}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
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
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Right, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Down, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_up = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Up, 0.2f, nullptr, &dockSpaceId);
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}