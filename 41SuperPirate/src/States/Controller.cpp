#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <States/Menu.h>

#include "Controller.h"
#include "Application.h"
#include "Globals.h"

Controller::Controller(StateMachine& machine) : 
	State(machine, States::DEFAULT),
	m_meshDisk(20.0f, Vector3f(20.0f, 20.0f, 0.0f), true, true, true, 30) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);

	//Vector3f position = { -static_cast<float>(Application::Width) * 0.5f + 100.0f, -static_cast<float>(Application::Height) * 0.5f + 100.0f, 0.0f };
	Vector3f position = { 0.0f, 0.0f, 0.0f };
	m_camera.lookAt(position, position + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(500.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);
}

Controller::~Controller() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Controller::fixedUpdate() {

}

void Controller::update() {
	Keyboard& keyboard = Keyboard::instance();
	Vector3f direction = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		direction += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		direction += Vector3f(0.0f, -1.0f, 0.0f);
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

	Mouse& mouse = Mouse::instance();

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

	XInputController& xInputController = XInputController::instance();
	m_state = 0;
	if (xInputController.buttonDown(XInputController::GamepadButton::DPAD_UP)) {
		m_state = 1;
	}

	if (xInputController.buttonDown(XInputController::GamepadButton::DPAD_RIGHT)) {
		m_state = 3;
	}

	if (xInputController.buttonDown(XInputController::GamepadButton::DPAD_DOWN)) {
		m_state = 5;
	}

	if (xInputController.buttonDown(XInputController::GamepadButton::DPAD_LEFT)) {
		m_state = 7;
	}

	if (xInputController.buttonDown(XInputController::GamepadButton::DPAD_RIGHT) && xInputController.buttonDown(XInputController::GamepadButton::DPAD_UP)) {
		m_state = 2;
	}

	if (xInputController.buttonDown(XInputController::GamepadButton::DPAD_RIGHT) && xInputController.buttonDown(XInputController::GamepadButton::DPAD_DOWN)) {
		m_state = 4;
	}

	if (xInputController.buttonDown(XInputController::GamepadButton::DPAD_LEFT) && xInputController.buttonDown(XInputController::GamepadButton::DPAD_DOWN)) {
		m_state = 6;
	}

	if (xInputController.buttonDown(XInputController::GamepadButton::DPAD_LEFT) && xInputController.buttonDown(XInputController::GamepadButton::DPAD_UP)) {
		m_state = 8;
	}

	if (xInputController.buttonDown(XInputController::GamepadButton::GAMEPAD_START)) {
		m_state = 9;
	}

	if (xInputController.buttonDown(XInputController::GamepadButton::GAMEPAD_BACK)) {
		m_state = 10;
	}

	if (xInputController.buttonDown(XInputController::GamepadButton::GAMEPAD_LEFT_THUMB)) {
		m_state = 11;
	}

	if (xInputController.buttonDown(XInputController::GamepadButton::GAMEPAD_RIGHT_THUMB)) {
		m_state = 12;
	}

	if (xInputController.buttonDown(XInputController::GamepadButton::GAMEPAD_LEFT_SHOULDER)) {
		m_state = 13;
	}

	if (xInputController.buttonDown(XInputController::GamepadButton::GAMEPAD_RIGHT_SHOULDER)) {
		m_state = 14;
	}

	if (xInputController.buttonDown(XInputController::GamepadButton::GAMEPAD_A)) {
		m_state = 15;
	}

	if (xInputController.buttonDown(XInputController::GamepadButton::GAMEPAD_B)) {
		m_state = 16;
	}

	if (xInputController.buttonDown(XInputController::GamepadButton::GAMEPAD_X)) {
		m_state = 17;
	}

	if (xInputController.buttonDown(XInputController::GamepadButton::GAMEPAD_Y)) {
		m_state = 18;
	}

	if (xInputController.buttonDown(XInputController::GamepadButton::GAMEPAD_LEFT_TRIGGER)) {
		m_state = 19;
	}

	if (xInputController.buttonDown(XInputController::GamepadButton::GAMEPAD_RIGHT_TRIGGER)) {
		m_state = 20;
	}
}

void Controller::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderPad(static_cast<float>(Application::Width) * 0.5f - 100.0f, static_cast<float>(Application::Height) * 0.5f - 100.0f, m_state);
	for (int i = 0; i < 12; i++) {
		renderButton(i * 50.0f + 50.0f, static_cast<float>(Application::Height) - 50.0f, m_state, i + 9);
	}
#if DEVBUILD
	if (m_drawUi)
		renderUi();
#endif
}

void Controller::renderPad(float x, float y, long value) {
	auto shader = Globals::shaderManager.getAssetPointer("color");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getOrthographicMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	for (int i = 1; i < 9; i++) {
		float xPos = (sinf(M_PI_4 * i - M_PI_4) * 80.0f) + 80.0f;
		float yPos = (cosf(M_PI_4 * i - M_PI_4) * 80.0f) + 80.0f;
		shader->loadMatrix("u_model", Matrix4f::Translate(x + xPos, y + yPos, 0.0f));
		shader->loadVector("u_color", value == i ? Vector4f(1.0f, 0.0f, 0.0f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		m_meshDisk.drawRaw();
		shader->loadMatrix("u_model", Matrix4f::Translate(x + xPos, y + yPos, 0.0f));
		shader->loadVector("u_color", Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
		m_meshDisk.drawRaw(true, 3.0f);
	}	
	shader->unuse();
}

void Controller::renderButton(float x, float y, long value, int index) {
	//std::cout << "Index: " << index << " Value: " << value << std::endl;
	auto shader = Globals::shaderManager.getAssetPointer("color");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getOrthographicMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", Matrix4f::Translate(x, y , 0.0f));
	shader->loadVector("u_color", value == index ? Vector4f(1.0f, 0.0f, 0.0f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	m_meshDisk.drawRaw();
	shader->loadMatrix("u_model", Matrix4f::Translate(x, y , 0.0f));
	shader->loadVector("u_color", Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	m_meshDisk.drawRaw(true, 3.0f);
	shader->unuse();
}

void Controller::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Controller::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}

	if (event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
	}
}

void Controller::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void Controller::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Controller::OnKeyDown(Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Controller::OnKeyUp(Event::KeyboardEvent& event) {

}

void Controller::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void Controller::renderUi() {
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