#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <States/Menu.h>

#include "Winston.h"
#include "Application.h"
#include "Globals.h"

Winston::Winston(StateMachine& machine) : State(machine, States::WINSTON) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 2.0f, 10.0f), Vector3f(0.0f, 2.0f, 10.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	m_world.fromObj("res/models/scene.obj");
	m_sphere.fromObj("res/models/sphere.obj");

	m_depthBuffer.create(Application::Width, Application::Height);
	m_depthBuffer.attachTexture2D(AttachmentTex::DEPTH24);
}

Winston::~Winston() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Winston::fixedUpdate() {

}

void Winston::update() {
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

	if (keyboard.keyDown(Keyboard::KEY_LEFT)) {
		m_bubblePos[0] -= m_dt;
	}

	if (keyboard.keyDown(Keyboard::KEY_RIGHT)) {
		m_bubblePos[0] += m_dt;
	}

	if (keyboard.keyDown(Keyboard::KEY_UP)) {
		m_bubblePos[2] -= m_dt;
	}

	if (keyboard.keyDown(Keyboard::KEY_DOWN)) {
		m_bubblePos[2] += m_dt;
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
}

void Winston::render() {

	m_depthBuffer.bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	renderSceneDepth();
	m_depthBuffer.unbind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderScene();
	glDepthMask(GL_FALSE);
	glCullFace(GL_FRONT);
	renderBubble();

	glCullFace(GL_BACK);
	renderBubble();

	glDepthMask(GL_TRUE);

	if (m_drawUi)
		renderUi();
}

void Winston::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Winston::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}

	if (event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
	}
}

void Winston::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void Winston::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Winston::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Winston::OnKeyUp(Event::KeyboardEvent& event) {

}

void Winston::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_depthBuffer.resize(Application::Width, Application::Height);
}

void Winston::renderUi() {
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
	ImGui::Image((ImTextureID)(intptr_t)m_depthBuffer.getDepthTexture(), ImVec2(200.0f, 200.0f), { 0.0f, 1.0f }, { 1.0f, 0.0f });
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Winston::renderBubble() {
	
	Matrix4f model = Matrix4f::Translate(m_bubblePos);

	auto shader = Globals::shaderManager.getAssetPointer("bubble");

	shader->use();
	shader->loadMatrix("modelView", m_camera.getViewMatrix() * model);
	shader->loadMatrix("projection", m_camera.getPerspectiveMatrix());
	shader->loadVector("color", Vector3f(0.0f, 0.9f, 1.0f));
	shader->loadFloat("alpha", 0.01f);
	shader->loadVector("screensize", Vector2f(static_cast<float>(Application::Width), static_cast<float>(Application::Height)));
	shader->loadFloat("near", 0.1f);
	shader->loadFloat("far", 100.0f);

	m_depthBuffer.bindDepthTexture(0u);
	m_sphere.drawRaw();
}

void Winston::renderSceneDepth() {

	auto shader = Globals::shaderManager.getAssetPointer("depth");

	shader->use();
	shader->loadMatrix("modelView", m_camera.getViewMatrix());
	shader->loadMatrix("projection", m_camera.getPerspectiveMatrix());
	shader->loadFloat("near", 0.1f);
	shader->loadFloat("far", 100.0f);
	m_world.drawRaw();
}

void Winston::renderScene() {
	auto shader = Globals::shaderManager.getAssetPointer("scene");
	
	shader->use();
	shader->loadMatrix("modelView", m_camera.getViewMatrix());
	shader->loadMatrix("projection", m_camera.getPerspectiveMatrix());

	shader->loadMatrix("view", m_camera.getViewMatrix());
	shader->loadVector("color", Vector3f(0.3f, 0.3f, 0.3f));
	shader->loadVector("lightDir", Vector3f(0.5f, -0.72f, -0.65f));
	m_world.drawRaw();
}