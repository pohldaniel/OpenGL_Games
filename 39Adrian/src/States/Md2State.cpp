#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <States/Menu.h>

#include "Md2State.h"
#include "Application.h"
#include "Globals.h"

std::string sModelNames[] =
{
	"Dr. Freak",
	"Blade",
	"Samourai",
	"HoboGoblin"
};

Md2State::Md2State(StateMachine& machine) : State(machine, States::DEFAULT) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 20.0f, 50.0f), Vector3f(0.0f, 19.5f, 50.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	md2Models[0].LoadModel("res/models/drfreak/drfreak.md2");
	md2Models[1].LoadModel("res/models/blade/Blade.md2");
	md2Models[2].LoadModel("res/models/samourai/Samourai.md2");
	md2Models[3].LoadModel("res/models/HoboGoblin/Model.md2");
	animationStateMain = md2Models[0].StartAnimation(STAND);

	//md2Converter.md2ToObj("data/models/dynamic/corpse/corpse.md2", "data/corpse_0.obj", "data/corpse.mtl", "/data/models/dynamic/corpse/corpse.tga", true, 0);
	//md2Converter.md2ToObj("data/models/dynamic/corpse/corpse.md2", "data/corpse_10.obj", "data/corpse.mtl", "/data/models/dynamic/corpse/corpse.tga", true, 10);

	//m_sequence.setStride(5u);
	//md2Converter.md2ToSequence("data/models/dynamic/corpse/corpse.md2", true, { 0.0f, -90.0f, 0.0f }, { 0.5f, 0.5f, 0.5f }, m_sequence);
	//m_sequence.loadSequenceGpu();

	m_sequence.setStride(5u);
	md2Converter.md2ToSequence("data/models/dynamic/hero/hero.md2", true, { 0.0f, -90.0f, 0.0f }, { 0.5f, 0.5f, 0.5f }, m_sequence);
	m_sequence.loadSequenceGpu();
}

Md2State::~Md2State() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Md2State::fixedUpdate() {

}

void Md2State::update() {
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

	md2Models[iCurrentModel].UpdateAnimation(&animationStateMain, m_dt);
}

void Md2State::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	Globals::textureManager.get("hero").bind(0);
	auto shader = Globals::shaderManager.getAssetPointer("shape");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	m_sequence.draw(index);
	shader->unuse();

	if (m_drawUi)
		renderUi();
}

void Md2State::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Md2State::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}

	if (event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
	}
}

void Md2State::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void Md2State::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Md2State::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Md2State::OnKeyUp(Event::KeyboardEvent& event) {

}

void Md2State::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void Md2State::renderUi() {
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
	ImGui::SliderInt("Index", &index, 0, m_sequence.getMeshes().size());
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}