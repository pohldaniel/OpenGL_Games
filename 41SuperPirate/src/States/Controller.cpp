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
	m_meshDisk(20.0f, Vector3f(20.0f, 20.0f, 0.0f), true, true, true, 30),
	m_meshQuad(Vector3f(-100.0f, -100.0f, 0.0f), Vector3f(200.0f, 200.0f, 0.0f), true, true, true) {
	
	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);

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
	

	XInputController* player1X = XInputController::GetPlayer(0);
	XInputController* player2X = XInputController::GetPlayer(1);

	RawInputController* player1 = RawInputController::GetPlayer(0);
	RawInputController* player2 = RawInputController::GetPlayer(1);
	
	m_RightStickX = player1 ? player1->getRightStickX() : 0.0f;
	m_RightStickY = player1 ? player1->getRightStickY() : 0.0f;

	m_LeftStickX = player1 ? player1->getLeftStickX() : 0.0f;
	m_LeftStickY = player1 ? player1->getLeftStickY() : 0.0f;


	m_RightStickX = !m_RightStickX && player2 ? player2->getRightStickX() : m_RightStickX;
	m_RightStickY = !m_RightStickY && player2 ? player2->getRightStickY() : m_RightStickY;

	m_LeftStickX = !m_LeftStickX && player2 ? player2->getLeftStickX() : m_LeftStickX;
	m_LeftStickY = !m_LeftStickY && player2 ? player2->getLeftStickY() : m_LeftStickY;


	m_RightStickX = !m_RightStickX && player1X ? player1X->getRightStickX() : 0.0f;
	m_RightStickY = !m_RightStickY && player1X ? player1X->getRightStickY() : 0.0f;

	m_LeftStickX = !m_LeftStickX && player1X ? player1X->getLeftStickX() : 0.0f;
	m_LeftStickY = !m_LeftStickY && player1X ? player1X->getLeftStickY() : 0.0f;


	m_RightStickX = !m_RightStickX && player2X ? player2X->getRightStickX() : m_RightStickX;
	m_RightStickY = !m_RightStickY && player1X ? player2X->getRightStickY() : m_RightStickY;

	m_LeftStickX = !m_LeftStickX && player2X ? player2X->getLeftStickX() : m_LeftStickX;
	m_LeftStickY = !m_LeftStickY && player2X ? player2X->getLeftStickY() : m_LeftStickY;
	
	
	m_state = 0;
	if ((player1X && player1X->buttonDown(GamepadButton::DPAD_UP)) ||
		(player2X && player2X->buttonDown(GamepadButton::DPAD_UP)) ||
		(player1 && player1->buttonDown(GamepadButton::DPAD_UP)) ||
		(player2 && player2->buttonDown(GamepadButton::DPAD_UP))) {
		m_state = 1;
	}

	if ((player1X && player1X->buttonDown(GamepadButton::DPAD_RIGHT)) ||
		(player2X && player2X->buttonDown(GamepadButton::DPAD_RIGHT)) ||
		(player1 && player1->buttonDown(GamepadButton::DPAD_RIGHT)) ||
		(player2 && player2->buttonDown(GamepadButton::DPAD_RIGHT))) {
		m_state = 3;
	}

	if ((player1X && player1X->buttonDown(GamepadButton::DPAD_DOWN)) ||
		(player2X && player2X->buttonDown(GamepadButton::DPAD_DOWN)) ||
		(player1 && player1->buttonDown(GamepadButton::DPAD_DOWN)) ||
		(player2 && player2->buttonDown(GamepadButton::DPAD_DOWN))) {
		m_state = 5;
	}

	if ((player1X && player1X->buttonDown(GamepadButton::DPAD_LEFT)) ||
		(player2X && player2X->buttonDown(GamepadButton::DPAD_LEFT)) ||
		(player1 && player1->buttonDown(GamepadButton::DPAD_LEFT)) ||
		(player2 && player2->buttonDown(GamepadButton::DPAD_LEFT))) {
		m_state = 7;
	}

	if ((player1X && player1X->buttonDown(GamepadButton::DPAD_UP_RIGHT)) ||
		(player2X && player2X->buttonDown(GamepadButton::DPAD_UP_RIGHT)) ||
		(player1 && player1->buttonDown(GamepadButton::DPAD_UP_RIGHT)) ||
		(player2 && player2->buttonDown(GamepadButton::DPAD_UP_RIGHT))) {
		m_state = 2;
	}

	if ((player1X && player1X->buttonDown(GamepadButton::DPAD_DOWN_RIGHT)) ||
		(player2X && player2X->buttonDown(GamepadButton::DPAD_DOWN_RIGHT)) ||
		(player1 && player1->buttonDown(GamepadButton::DPAD_DOWN_RIGHT)) ||
		(player2 && player2->buttonDown(GamepadButton::DPAD_DOWN_RIGHT))) {
		m_state = 4;
	}

	if ((player1X && player1X->buttonDown(GamepadButton::DPAD_DOWN_LEFT)) ||
		(player2X && player2X->buttonDown(GamepadButton::DPAD_DOWN_LEFT)) ||
		(player1 && player1->buttonDown(GamepadButton::DPAD_DOWN_LEFT)) ||
		(player2 && player2->buttonDown(GamepadButton::DPAD_DOWN_LEFT))) {
		m_state = 6;
	}

	if ((player1X && player1X->buttonDown(GamepadButton::DPAD_UP_LEFT)) ||
		(player2X && player2X->buttonDown(GamepadButton::DPAD_UP_LEFT)) ||
		(player1 && player1->buttonDown(GamepadButton::DPAD_UP_LEFT)) ||
		(player2 && player2->buttonDown(GamepadButton::DPAD_UP_LEFT))) {
		m_state = 8;
	}

	if ((player1X && player1X->buttonDown(GamepadButton::GAMEPAD_START)) ||
		(player2X && player2X->buttonDown(GamepadButton::GAMEPAD_START)) ||
		(player1 && player1->buttonDown(GamepadButton::GAMEPAD_START)) ||
		(player2 && player2->buttonDown(GamepadButton::GAMEPAD_START))) {
		m_state = 9;
	}

	if ((player1X && player1X->buttonDown(GamepadButton::GAMEPAD_BACK)) ||
		(player2X && player2X->buttonDown(GamepadButton::GAMEPAD_BACK)) ||
		(player1 && player1->buttonDown(GamepadButton::GAMEPAD_BACK)) ||
		(player2 && player2->buttonDown(GamepadButton::GAMEPAD_BACK))) {
		m_state = 10;
	}

	if ((player1X && player1X->buttonDown(GamepadButton::GAMEPAD_LEFT_THUMB)) ||
		(player2X && player2X->buttonDown(GamepadButton::GAMEPAD_LEFT_THUMB)) ||
		(player1 && player1->buttonDown(GamepadButton::GAMEPAD_LEFT_THUMB)) ||
		(player2 && player2->buttonDown(GamepadButton::GAMEPAD_LEFT_THUMB))) {
		m_state = 11;
	}

	if ((player1X && player1X->buttonDown(GamepadButton::GAMEPAD_RIGHT_THUMB)) ||
		(player2X && player2X->buttonDown(GamepadButton::GAMEPAD_RIGHT_THUMB)) ||
		(player1 && player1->buttonDown(GamepadButton::GAMEPAD_RIGHT_THUMB)) ||
		(player2 && player2->buttonDown(GamepadButton::GAMEPAD_RIGHT_THUMB))) {
		m_state = 12;
	}

	if ((player1X && player1X->buttonDown(GamepadButton::GAMEPAD_LEFT_SHOULDER)) ||
		(player2X && player2X->buttonDown(GamepadButton::GAMEPAD_LEFT_SHOULDER)) ||
		(player1 && player1->buttonDown(GamepadButton::GAMEPAD_LEFT_SHOULDER)) ||
		(player2 && player2->buttonDown(GamepadButton::GAMEPAD_LEFT_SHOULDER))) {
		m_state = 13;
	}

	if ((player1X && player1X->buttonDown(GamepadButton::GAMEPAD_RIGHT_SHOULDER)) ||
		(player2X && player2X->buttonDown(GamepadButton::GAMEPAD_RIGHT_SHOULDER)) ||
		(player1 && player1->buttonDown(GamepadButton::GAMEPAD_RIGHT_SHOULDER)) ||
		(player2 && player2->buttonDown(GamepadButton::GAMEPAD_RIGHT_SHOULDER))) {
		m_state = 14;
	}

	if ((player1X && player1X->buttonDown(GamepadButton::GAMEPAD_A)) ||
		(player2X && player2X->buttonDown(GamepadButton::GAMEPAD_A)) ||
		(player1 && player1->buttonDown(GamepadButton::GAMEPAD_A)) ||
		(player2 && player2->buttonDown(GamepadButton::GAMEPAD_A))) {
		m_state = 15;
		if(player1X && player1X->buttonDown(GamepadButton::GAMEPAD_A))
			player1X->setVibration(1.0f, 1.0f);

		if (player2X && player2X->buttonDown(GamepadButton::GAMEPAD_A))
			player2X->setVibration(1.0f, 1.0f);
	}

	if ((player1X && player1X->buttonDown(GamepadButton::GAMEPAD_B)) ||
		(player2X && player2X->buttonDown(GamepadButton::GAMEPAD_B)) ||
		(player1 && player1->buttonDown(GamepadButton::GAMEPAD_B)) ||
		(player2 && player2->buttonDown(GamepadButton::GAMEPAD_B))) {
		m_state = 16;
		if (player1X && player1X->buttonDown(GamepadButton::GAMEPAD_B))
			player1X->setVibration(0.0f, 0.0f);

		if (player2X && player2X->buttonDown(GamepadButton::GAMEPAD_B))
			player2X->setVibration(0.0f, 0.0f);
	}

	if ((player1X && player1X->buttonDown(GamepadButton::GAMEPAD_X)) ||
		(player2X && player2X->buttonDown(GamepadButton::GAMEPAD_X)) ||
		(player1 && player1->buttonDown(GamepadButton::GAMEPAD_X)) ||
		(player2 && player2->buttonDown(GamepadButton::GAMEPAD_X))) {
		m_state = 17;
	}

	if ((player1X && player1X->buttonDown(GamepadButton::GAMEPAD_Y)) ||
		(player2X && player2X->buttonDown(GamepadButton::GAMEPAD_Y)) ||
		(player1 && player1->buttonDown(GamepadButton::GAMEPAD_Y)) ||
		(player2 && player2->buttonDown(GamepadButton::GAMEPAD_Y))) {
		m_state = 18;
	}

	if ((player1X && player1X->buttonDown(GamepadButton::GAMEPAD_LEFT_TRIGGER)) ||
		(player2X && player2X->buttonDown(GamepadButton::GAMEPAD_LEFT_TRIGGER)) ||
		(player1 && player1->buttonDown(GamepadButton::GAMEPAD_LEFT_TRIGGER)) ||
		(player2 && player2->buttonDown(GamepadButton::GAMEPAD_LEFT_TRIGGER))) {
		m_state = 19;
	}
	//std::cout << player1X << "  " << player2X << "  " << player1 << "  " << player2 << std::endl;
	if ((player1X && player1X->buttonDown(GamepadButton::GAMEPAD_RIGHT_TRIGGER)) ||
		(player2X && player2X->buttonDown(GamepadButton::GAMEPAD_RIGHT_TRIGGER)) ||
		(player1 && player1->buttonDown(GamepadButton::GAMEPAD_RIGHT_TRIGGER)) ||
		(player2 && player2->buttonDown(GamepadButton::GAMEPAD_RIGHT_TRIGGER))) {
	
		m_state = 20;
	}
}

void Controller::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderPad(static_cast<float>(Application::Width) * 0.5f - 100.0f, static_cast<float>(Application::Height) * 0.5f - 100.0f, m_state);
	for (int i = 0; i < 12; i++) {
		renderButton(i * 50.0f + 50.0f, static_cast<float>(Application::Height) - 50.0f, m_state, i + 9);
	}
	renderRectangle(static_cast<float>(Application::Width) * 0.5f + 300.0f, static_cast<float>(Application::Height) * 0.5f, m_LeftStickX, m_LeftStickY);
	renderRectangle(static_cast<float>(Application::Width) * 0.5f - 300.0f, static_cast<float>(Application::Height) * 0.5f, m_RightStickX, m_RightStickY);
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
	//std::cout << "VALUE: " << value << std::endl;
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

void Controller::renderRectangle(float x, float y, float stickX, float stickY) {
	
	auto shader = Globals::shaderManager.getAssetPointer("color");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getOrthographicMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", Matrix4f::Translate(x, y, 0.0f));
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	m_meshQuad.drawRaw();


	shader->loadVector("u_color", Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	m_meshQuad.drawRaw(true, 3.0f);

	shader->loadMatrix("u_model", Matrix4f::Translate((x - 20.0f) + stickX * 50.0f, (y - 20.0f) + stickY * 50.0f, 0.0f));
	shader->loadVector("u_color", Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	m_meshDisk.drawRaw();

	shader->unuse();
}

void Controller::OnMouseMotion(const Event::MouseMoveEvent& event) {

}

void Controller::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}

	if (event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
	}
}

void Controller::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void Controller::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void Controller::OnKeyDown(const Event::KeyboardEvent& event) {
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

void Controller::OnKeyUp(const Event::KeyboardEvent& event) {

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