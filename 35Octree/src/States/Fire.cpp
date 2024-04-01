#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>
#include <engine/Material.h>

#include "Fire.h"

#include "Application.h"
#include "Globals.h"

Fire::Fire(StateMachine& machine) : State(machine, States::FIRE) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 2.0f, -10.0f), Vector3f(0.0f, 2.0f, -10.0f) + Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(5.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	
	Material::AddTexture("res/textures/Flame_Particle.png");
	Material::AddTexture("res/textures/Log_pine_color.png");
	Material::AddTexture("res/textures/Log_pine_normal.png");
	Material::AddTexture("res/textures/Grass.png");
	Material::AddTexture("res/textures/Grass_normal.png");

	Material::AddMaterial();
	Material::GetMaterials().back().addTexture(Material::GetTextures()[1], 0u);
	Material::GetMaterials().back().addTexture(Material::GetTextures()[2], 1u);

	Material::AddMaterial();
	Material::GetMaterials().back().addTexture(Material::GetTextures()[3], 0u);
	Material::GetMaterials().back().addTexture(Material::GetTextures()[4], 1u);


	m_logPine.loadModel("res/models/Log_pine.obj", false, false, true);
	m_logPine.getMesh(0)->setMaterialIndex(0);
	m_grass.loadModel("res/models/Grass.obj", false, false, true);
	m_grass.getMesh(0)->setMaterialIndex(1);

	m_root = new SceneNodeLC();
	Entity* child = m_root->addChild<Entity, AssimpModel>(m_logPine);
	child->setScale(0.1f, 0.1f, 0.1f);
	child->setPosition(-3.0f, 0.1f, 0.0f);
	child->setOrientation(180.0f, 0.0f, 0.0f);
	entities.push_back(child);

	child = m_root->addChild<Entity, AssimpModel>(m_logPine);
	child->setScale(0.1f, 0.1f, 0.1f);
	child->setPosition(3.0f, 0.1f, 0.0f);
	child->setOrientation(180.0f, 0.0f, 0.0f);
	entities.push_back(child);

	child = m_root->addChild<Entity, AssimpModel>(m_logPine);
	child->setScale(0.1f, 0.1f, 0.1f);
	child->setPosition(0.0f, 0.1f, -3.0f);
	child->setOrientation(180.0f, 90.0f, 0.0f);
	entities.push_back(child);

	child = m_root->addChild<Entity, AssimpModel>(m_logPine);
	child->setScale(0.1f, 0.1f, 0.1f);
	child->setPosition(0.0f, 0.1f, 3.0f);
	child->setOrientation(180.0f, 90.0f, 0.0f);
	entities.push_back(child);

	child = m_root->addChild<Entity, AssimpModel>(m_grass);
	child->setScale(0.5f, 0.5f, 0.5f);
	child->setPosition(0.0f, -0.3f, 0.0f);
	entities.push_back(child);

	m_lightPosition = Vector3f(0.0f, 1.0f, 0.0f); 
	m_fireUp = true;
}

Fire::~Fire() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Fire::fixedUpdate() {

}

void Fire::update() {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f direction = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		direction += Vector3f(0.0f, 0.0f, 1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		direction += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		direction += Vector3f(-1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		direction += Vector3f(1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		direction += Vector3f(0.0f, -1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		direction += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}

	Mouse &mouse = Mouse::instance();

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xDelta();
		dy = mouse.yDelta();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotate(dx, dy);
		}

		if (move) {
			m_camera.move(direction * m_dt);
		}
	}

	if (m_fireUp){
		m_lightPosition[1] += 0.6f * m_dt;
		if (m_lightPosition[1] > 2.0f){
			m_fireUp = false;
		}
	}else{
		m_lightPosition[1] -= 0.6f * m_dt;
		if (m_lightPosition[1] < 1.5f){
			m_fireUp = true;
		}
	}

	m_flame.update(m_dt);
}

void Fire::render() {

	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto shader = Globals::shaderManager.getAssetPointer("model");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadVector("u_lightPos", m_lightPosition);
	shader->loadInt("u_diffuse", 0);
	shader->loadInt("u_normal", 1);
	for (auto&& entity : entities) {
		shader->loadMatrix("u_model", entity->getWorldTransformation());
		entity->draw();
	}
	
	glDepthMask(false);
	shader = Globals::shaderManager.getAssetPointer("fire");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", Matrix4f::IDENTITY);
	shader->loadFloat("u_resolutionY", static_cast<float>(Application::Height));
	shader->loadInt("u_diffuse", 0);
	Globals::textureManager.get("flame").bind(0u);
	m_flame.draw();
	shader->unuse();
	glDepthMask(true);

	if (m_drawUi)
		renderUi();
}

void Fire::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Fire::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Fire::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void Fire::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Fire::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void Fire::OnKeyUp(Event::KeyboardEvent& event) {
	
}

void Fire::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void Fire::renderUi() {
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