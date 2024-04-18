#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>
#include <States/Menu.h>

#include "SSSOGLP.h"
#include "Application.h"
#include "Globals.h"

SSSOGLP::SSSOGLP(StateMachine& machine) : State(machine, States::RAYMARCH) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.01f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, -3.0f), Vector3f(0.0f, 0.0f, 0.0f) + Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(5.0f);

	m_trackball.reshape(Application::Width, Application::Height);
	applyTransformation(m_trackball);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	m_buddha.loadModel("res/models/buddha.obj", Vector3f(1.0f, 0.0f, 0.0f), 90.0f, Vector3f(0.0f, 0.0f, 0.0f), 1.0f);
	m_buddha.markForDelete();
	m_bunny.loadModel("res/models/bunny.obj", Vector3f(0.0f, 1.0f, 0.0f), 0.0f, Vector3f(0.0f, 0.0f, 0.0f), 1.5f);
	m_bunny.markForDelete();
	m_dragon.loadModel("res/models/dragon.obj", Vector3f(0.0f, 1.0f, 0.0f), 0.0f, Vector3f(0.0f, 0.0f, 0.0f), 0.05f);
	m_dragon.markForDelete();

	m_depth.create(DEPTH_RES, DEPTH_RES);
	m_depth.attachTexture2D(AttachmentTex::DEPTH32F);
	m_depth.attachRenderbuffer(AttachmentRB::RED);
	m_center = Vector3f(0.0f, 0.108352f, 0.0f);
	radius = bs_rad * 1.2f;

	auto shader = Globals::shaderManager.getAssetPointer("main_oglp");
	shader->use();
	for (int i = 0; i != 32; ++i) {
		float u = std::rand() / float(RAND_MAX);
		float v = std::rand() / float(RAND_MAX);
		float x = float(std::sqrt(v) * std::cos(2 * 3.1415 * u));
		float y = float(std::sqrt(v) * std::sin(2 * 3.1415 * u));
		shader->loadVector(std::string("DepthOffs[" + std::to_string(i) + "]").c_str(), Vector2f(x, y));
	}
	shader->unuse();
	Globals::clock.restart();
}

SSSOGLP::~SSSOGLP() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void SSSOGLP::fixedUpdate() {

}

void SSSOGLP::update() {
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



	float time = Globals::clock.getElapsedTimeSec();
	Orbiting(m_lightView, m_center, bs_rad * 10.0f, time / 23.0f * TWO_PI, (35 + std::sinf(TWO_PI * (time / 31.0f)) * 50.0f) * PI_ON_180);
	m_lightPos = getPosition(m_lightView, m_lightPos);
	float lgt_tgt_dist = Vector3f::Length(m_center, m_lightPos);
	m_lightProj = Matrix4f::Perspective(std::asinf(radius / lgt_tgt_dist) * 2.0f * _180_ON_PI, 1.0f, lgt_tgt_dist - radius, lgt_tgt_dist + radius);

	Orbiting(m_view, m_center, bs_rad * 2.8f, time / 17.0f * TWO_PI, (std::sinf(TWO_PI * (time / 19.0f)) * 80.0f) * PI_ON_180);
	m_camPos = getPosition(m_view, m_camPos);

	float cam_tgt_dist = Vector3f::Length(m_center, m_camPos) * 0.95f;
	m_proj = Matrix4f::Perspective(std::asinf(radius / cam_tgt_dist) * 2.0f * _180_ON_PI, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
	m_model = Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), std::sinf(TWO_PI * (time / 21.0f)) * 25.0f) * Matrix4f::Translate(0.0f, -radius * 0.25f, 0.0f);
}

void SSSOGLP::render() {
	
	m_depth.bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto shader = Globals::shaderManager.getAssetPointer("depth_oglp");
	shader->use();
	shader->loadMatrix("u_lightViewProj", m_lightProj * m_lightView);
	shader->loadMatrix("u_model", m_model);

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
	m_depth.unbind();

	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader = Globals::shaderManager.getAssetPointer("main_oglp");
	shader->use();
	shader->loadMatrix("u_viewProj", m_proj *  m_view);
	shader->loadMatrix("u_lightViewProj", m_lightProj * m_lightView);
	shader->loadMatrix("u_model", m_model);
	shader->loadVector("u_lightPos", m_lightPos);
	shader->loadVector("u_camPos", m_camPos);
	shader->loadVector("u_color", Vector3f(m_color[0], m_color[1], m_color[2]));
	shader->loadInt("DepthMap", 0);
	m_depth.bindDepthTexture(0u);

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

void SSSOGLP::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void SSSOGLP::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);
	}
	else if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void SSSOGLP::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);
	}
	else if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void SSSOGLP::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void SSSOGLP::OnKeyDown(Event::KeyboardEvent& event) {
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

void SSSOGLP::OnKeyUp(Event::KeyboardEvent& event) {

}

void SSSOGLP::applyTransformation(TrackBall& arc) {
	m_transform.fromMatrix(arc.getTransform());
}

void SSSOGLP::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.01f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_trackball.reshape(Application::Width, Application::Height);
}

void SSSOGLP::renderUi() {
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

	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());	
	int currentModel = model;
	if (ImGui::Combo("Model", &currentModel, "Bunny\0Dragon\0Buddha\0\0")) {
		model = static_cast<Model>(currentModel);
	}
	ImGui::ColorEdit3("color", m_color);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

const Matrix4f& SSSOGLP::Orbiting(Matrix4f& mtx, const Vector3f& target, float radius, float azinuthRad, float elevationRad) {
	Vector3f z = { std::cosf(elevationRad) * std::cosf(azinuthRad), std::sinf(elevationRad), std::cosf(elevationRad) * -std::sinf(azinuthRad) };
	Vector3f x = { -std::sinf(azinuthRad), 0.0f,  -std::cosf(azinuthRad) };
	Vector3f y = Vector3f::Cross(z, x);
	
	mtx[0][0] = x[0];
	mtx[0][1] = y[0];
	mtx[0][2] = z[0];
	mtx[0][3] = 0.0f;

	mtx[1][0] = x[1];
	mtx[1][1] = y[1];
	mtx[1][2] = z[1];
	mtx[1][3] = 0.0f;

	mtx[2][0] = x[2];
	mtx[2][1] = y[2];
	mtx[2][2] = z[2];
	mtx[2][3] = 0.0f;

	mtx[3][0] = -((Vector3f::Dot(x, z) * radius) + Vector3f::Dot(x, target));
	mtx[3][1] = -((Vector3f::Dot(y, z) * radius) + Vector3f::Dot(y, target));
	mtx[3][2] = -((Vector3f::Dot(z, z) * radius) + Vector3f::Dot(z, target));
	mtx[3][3] = 1.0f;

	return mtx;
}

const Vector3f& SSSOGLP::getPosition(Matrix4f& mtx, Vector3f& pos) {
	pos[0] = -(mtx[3][0] * mtx[0][0] + mtx[3][1] * mtx[0][1] + mtx[3][2] * mtx[0][2]);
	pos[1] = -(mtx[3][0] * mtx[1][0] + mtx[3][1] * mtx[1][1] + mtx[3][2] * mtx[1][2]);
	pos[2] = -(mtx[3][0] * mtx[2][0] + mtx[3][1] * mtx[2][1] + mtx[3][2] * mtx[2][2]);
	return pos;
}