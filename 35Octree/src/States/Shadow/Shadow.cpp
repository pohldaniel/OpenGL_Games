#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>
#include <States/Menu.h>

#include "Shadow.h"
#include "Application.h"
#include "Globals.h"

Shadow::Shadow(StateMachine& machine) : State(machine, States::SHADOW) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 20.0f, 500.0f), Vector3f(0.0f, 20.0f, 500.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(100.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);
	glPolygonOffset(2.0f, 4.0f);

	m_depthRT.create(DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
	m_depthRT.attachTexture2D(AttachmentTex::DEPTH32F);
	Texture::SetCompareFunc(m_depthRT.getDepthTexture(), GL_LESS);

	lightProjection = Matrix4f::Perspective(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 800.0f);
	m_armadillo.LoadFromVBM("res/models/armadillo_low.vbm", 0, 1, 2);
	Globals::clock.restart();
}

Shadow::~Shadow() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Shadow::fixedUpdate() {

}

void Shadow::update() {
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
}

void Shadow::render() {
	
	float time = Globals::clock.getElapsedTimeSec();
	float t = float(GetTickCount() & 0xFFFF) / float(0xFFFF);
	lightPosition = { sinf(t * 6.0f * 3.141592f) * 300.0f, 200.0f, cosf(t * 4.0f * 3.141592f) * 100.0f + 250.0f };
	lightView = Matrix4f::LookAt(lightPosition, Vector3f(0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	model.rotate(Vector3f(0.0f, 1.0f, 0.0f), t * 720.0f);

	m_depthRT.bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_POLYGON_OFFSET_FILL);

	auto shader = Globals::shaderManager.getAssetPointer("shadow");
	shader->use();
	shader->loadMatrix("mvp", lightProjection * lightView * model);
	m_armadillo.Render();
	Globals::shapeManager.get("floor_shadow").drawRaw();
	shader->unuse();
	glDisable(GL_POLYGON_OFFSET_FILL);	
	m_depthRT.unbind();
	
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);



	shader = Globals::shaderManager.getAssetPointer("shadow_base");
	shader->use();
	shader->loadMatrix("model", model);
	shader->loadMatrix("view", m_camera.getViewMatrix());
	shader->loadMatrix("proj", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("shadow_matrix", Matrix4f::BIAS * lightProjection * lightView);
	shader->loadVector("lightPos", lightPosition);
	shader->loadVector("mat_ambient", Vector3f(0.1f, 0.0f, 0.2f));
	shader->loadVector("mat_diffuse", Vector3f(0.3f, 0.2f, 0.8f));
	shader->loadVector("mat_specular", Vector3f(1.0f, 1.0f, 1.0f));
	shader->loadFloat("mat_specular_power", 25.0f);
	shader->loadInt("u_shadowMap", 0);

	m_depthRT.bindDepthTexture(0);
	m_armadillo.Render();

	shader->loadVector("mat_ambient", Vector3f(0.1f, 0.1f, 0.1f));
	shader->loadVector("mat_diffuse", Vector3f(0.1f, 0.5f, 0.1f));
	shader->loadVector("mat_specular", Vector3f(0.1f, 0.1f, 0.1f));
	shader->loadFloat("mat_specular_power", 3.0f);

	Globals::shapeManager.get("floor_shadow").drawRaw();

	shader->unuse();


	if (m_drawUi)
		renderUi();
}

void Shadow::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Shadow::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Shadow::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void Shadow::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Shadow::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Shadow::OnKeyUp(Event::KeyboardEvent& event) {

}

void Shadow::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void Shadow::renderUi() {
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