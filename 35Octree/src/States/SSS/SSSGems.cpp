#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>

#include "SSSGems.h"
#include "Application.h"
#include "Globals.h"

SSSGems::SSSGems(StateMachine& machine) : State(machine, States::RAYMARCH) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.01f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, -3.0f), Vector3f(0.0f, 0.0f, 0.0f) + Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(5.0f);

	glClearColor(0.33333333f, 0.33333333f, 0.33333333f, 1.0f);
	glClearDepth(1.0f);
	
	m_buddha.loadModel("res/models/buddha.obj", Vector3f(1.0f, 0.0f, 0.0f), 90.0f, Vector3f(0.0f, 0.0f, 0.0f), 1.0f);
	m_bunny.loadModel("res/models/bunny.obj");
	m_dragon.loadModel("res/models/dragon_low.obj");

	m_distance.create(SHADOW_RES, SHADOW_RES);
	//m_distance.create(Application::Width, Application::Height);
	m_distance.attachTexture2D(AttachmentTex::RGBA32F);
	m_distance.attachTexture2D(AttachmentTex::DEPTH16);

	m_lightInfo.dir = Vector3f::Normalize(Vector3f(0.0f, -1.0f, -1.0f));
	m_lightInfo.size = 7.0f;
	m_lightInfo.proj = Matrix4f::Orthographic(-m_lightInfo.size / 2.0f, m_lightInfo.size / 2.0f, -m_lightInfo.size / 2.0f, m_lightInfo.size / 2.0f, -10.0f, 20.0f);
	m_lightInfo.view = Matrix4f::LookAt(-m_lightInfo.dir * 1.0f, Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f,0.0f));
	m_lightInfo.projView = m_lightInfo.proj * m_lightInfo.view;
	m_lightInfo.biasProjView = Matrix4f::BIAS * m_lightInfo.projView;

	m_trackball.reshape(Application::Width, Application::Height);
	//m_trackball.setDollyPosition(-2.5f);

	applyTransformation(m_trackball);
}

SSSGems::~SSSGems() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void SSSGems::fixedUpdate() {

}

void SSSGems::update() {
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

	m_trackball.idle();
	applyTransformation(m_trackball);
}

void SSSGems::render() {
	
	m_distance.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 1, maxDistance);
	auto shader = Globals::shaderManager.getAssetPointer("depth_gems");
	shader->use();
	shader->loadMatrix("u_lightProj", m_lightInfo.proj);
	shader->loadMatrix("u_lightView", m_lightInfo.view);
	shader->loadMatrix("u_model", m_transform.getTransformationMatrix());

	switch (model) {
	case Model::BUNNY:
		m_bunny.drawRaw();
		break;
	case Model::DRAGON:
		m_dragon.drawRaw();
		break;
	case Model::BUDDHA:
		m_buddha.drawRaw();
		break;
	}

	shader->unuse();
	m_distance.unbind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader = Globals::shaderManager.getAssetPointer("main_gems");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", m_transform.getTransformationMatrix());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transform.getTransformationMatrix()));
	shader->loadFloat("u_thikness", m_thikness);

	shader->loadMatrix("light.biasProjView", m_lightInfo.biasProjView);
	shader->loadMatrix("light.projView", m_lightInfo.projView);
	shader->loadMatrix("light.view", m_lightInfo.view);
	shader->loadVector("light.dir", m_lightInfo.dir);

	shader->loadInt("texDepth", 0);
	shader->loadInt("texShadow", 1);
	
	m_distance.bindColorTexture(0u, 0u);
	m_distance.bindDepthTexture(1u);

	switch (model) {
		case Model::BUNNY:
			m_bunny.drawRaw();
			break;
		case Model::DRAGON:
			m_dragon.drawRaw();
			break;
		case Model::BUDDHA:
			m_buddha.drawRaw();
			break;
	}

	shader->unuse();

	if (m_drawUi)
		renderUi();
}

void SSSGems::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void SSSGems::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void SSSGems::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void SSSGems::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void SSSGems::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void SSSGems::OnKeyUp(Event::KeyboardEvent& event) {

}

void SSSGems::applyTransformation(TrackBall& arc) {
	m_transform.fromMatrix(arc.getTransform());
}

void SSSGems::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.01f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	//m_distance.resize(Application::Width, Application::Height);
	m_trackball.reshape(Application::Width, Application::Height);
}

void SSSGems::renderUi() {
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
	ImGui::SliderFloat("Scattering Power", &m_thikness, 0.0f, 10.0f);

	int currentModel = model;
	if (ImGui::Combo("Model", &currentModel, "Bunny\0Dragon\0Buddha\0\0")) {
		model = static_cast<Model>(currentModel);
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}