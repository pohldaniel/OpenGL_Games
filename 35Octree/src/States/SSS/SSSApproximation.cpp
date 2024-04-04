#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>

#include "SSSApproximation.h"
#include "Application.h"
#include "Globals.h"

SSSApproximation::SSSApproximation(StateMachine& machine) : State(machine, States::RAYMARCH) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(-14.0f, 20.0f, -14.0f), Vector3f(0.0f, 15.0f, 0.0f) + Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

	glClearColor(0.33333333f, 0.33333333f, 0.33333333f, 1.0f);
	glClearDepth(1.0f);
	m_background.resize(Application::Width, Application::Height);
	m_background.setLayer(std::vector<BackgroundLayer>{
		{ &Globals::textureManager.get("forest_1"), 1, 1.0f },
		{ &Globals::textureManager.get("forest_2"), 1, 2.0f },
		{ &Globals::textureManager.get("forest_3"), 1, 3.0f },
		{ &Globals::textureManager.get("forest_4"), 1, 4.0f },
		{ &Globals::textureManager.get("forest_5"), 1, 5.0f }});
	m_background.setSpeed(0.005f);

	m_model.loadModel("res/models/statue.obj");
	m_model.initShader();

	m_lightPos = Vector3f(-15.0f, 20.0f, -15.0f);
	m_lightView = Matrix4f::LookAt(m_lightPos, Vector3f(0.0f, 15.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_lightViewInverse = Matrix4f::InvLookAt(m_lightPos, Vector3f(0.0f, 15.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_lightProjection = m_camera.getPerspectiveMatrix();
	m_lightViewProjection = m_camera.getPerspectiveMatrix() * m_lightView;
	m_lightTexcoord = Matrix4f::BIAS * m_lightViewProjection;

	m_irradiance.create(Application::Width, Application::Height);
	m_irradiance.attachTexture2D(AttachmentTex::RGBA);
	m_irradiance.attachTexture2D(AttachmentTex::DEPTH24);

	m_distance.create(Application::Width, Application::Height);
	m_distance.attachTexture2D(AttachmentTex::RGBA);
	m_distance.attachTexture2D(AttachmentTex::RED32F);
	m_distance.attachTexture2D(AttachmentTex::DEPTH24);
}

SSSApproximation::~SSSApproximation() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void SSSApproximation::fixedUpdate() {

}

void SSSApproximation::update() {
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

	m_background.update(m_dt);
}

void SSSApproximation::render() {

	m_irradiance.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	auto shader = Globals::shaderManager.getAssetPointer("irradiance");
	shader->use();
	shader->loadMatrix("u_model", Matrix4f::IDENTITY);
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(Matrix4f::IDENTITY));
	shader->loadMatrix("u_lightViewProj", m_lightViewProjection);
	shader->loadMatrix("u_lightView", m_lightView);
	shader->loadMatrix("u_lightViewInverse", m_lightViewInverse);
	shader->loadVector("u_lightPos", m_lightPos);
	shader->loadInt("u_texture", 0);
	m_model.getMesh(2)->drawRaw();

	shader->unuse();
	m_irradiance.unbind();

	m_distance.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_background.draw();
	glClearBufferfv(GL_COLOR, 1, maxDistance);

	shader = Globals::shaderManager.getAssetPointer("distance");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_lightViewProj", m_lightViewProjection);
	shader->loadMatrix("u_lightView", m_lightView);
	shader->loadVector("u_lightPos", m_lightPos);

	m_model.getMesh(2)->drawRaw();

	shader->unuse();
	m_distance.unbind();

	/*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glDisable(GL_DEPTH_TEST);
	shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadInt("u_screen_texture", 0);
	shader->loadInt("u_depth_texture", 1);
	m_distance.bindColorTexture(0u, 0u);
	m_distance.bindColorTexture(1u, 1u);

	Globals::shapeManager.get("quad").drawRaw();
	shader->unuse();*/

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader = Globals::shaderManager.getAssetPointer("approximate");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_invView", m_camera.getInvViewMatrix());
	shader->loadMatrix("u_model", Matrix4f::IDENTITY);
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(Matrix4f::IDENTITY));

	shader->loadMatrix("u_lightView", m_lightView);
	shader->loadMatrix("u_lightProj", m_lightProjection);
	shader->loadMatrix("u_lightViewProj", m_lightViewProjection);
	shader->loadMatrix("u_lightTexcoord", m_lightTexcoord);
	shader->loadVector("u_lightPos", m_lightPos);
	shader->loadVector("u_camPos", m_camera.getPosition());
	shader->loadBool("m_useDiffuseColor", m_useDiffuse);

	shader->loadInt("u_texture", 0);
	shader->loadInt("u_irradiance", 1);
	shader->loadInt("u_distance", 2);

	m_irradiance.bindColorTexture(0u, 1u);
	m_distance.bindColorTexture(1u, 2u);

	m_model.getMesh(2)->drawRaw();
	shader->unuse();
	if (m_drawUi)
		renderUi();
}

void SSSApproximation::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void SSSApproximation::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void SSSApproximation::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void SSSApproximation::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void SSSApproximation::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void SSSApproximation::OnKeyUp(Event::KeyboardEvent& event) {

}

void SSSApproximation::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_distance.resize(Application::Width, Application::Height);
}

void SSSApproximation::renderUi() {
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
	ImGui::Checkbox("Use Diffuse", &m_useDiffuse);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}