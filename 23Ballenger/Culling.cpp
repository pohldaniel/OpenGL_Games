#include <time.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Culling.h"
#include "Constants.h"
#include "Application.h"

Culling::Culling(StateMachine& machine) : State(machine, CurrentState::SHAPEINTERFACE) {
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f * _180_ON_PI, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	
	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(-2.5f);
	applyTransformation(m_trackball);


	m_terrain.init("Levels/terrain01.raw", false);
	const float* heights = m_terrain.getHeightMap().getHeights();
	float minY = heights[0];
	float maxY = minY;
	for (int i = 1; i < m_terrain.getHeightMap().getWidth() * m_terrain.getHeightMap().getHeight(); i++) {
		if (heights[i] < minY) minY = heights[i];
		if (heights[i] > maxY) maxY = heights[i];
	}

	Vector3f  Min = Vector3f(0.0f, minY, 0.0f) ;
	Vector3f  Max = Vector3f(1024.0f, maxY, 1024.0f) ;

	m_quadTree.init(m_terrain.getPositions().data(), m_terrain.getIndexBuffer().data(), m_terrain.getIndexBuffer().size(), Min, Max, 64.0f);

	Globals::shaderManager.loadShader("culling", "res/terrain.vert", "res/terrain.frag");

	Wireframe = false;
	RenderAABB = false;
	RenderTree2D = false;
	VisualizeRenderingOrder = false;
	SortVisibleGeometryNodes = true;
	VisibilityCheckingPerformanceTest = false;

	Depth = -1;

	float Height = m_terrain.heightAt(512.0f, 512.0f);
	m_camera.lookAt(Vector3f(512.0f, Height + 1.75f, 512.0f), Vector3f(512.0f, Height + 1.75f, 512.0f -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
}

Culling::~Culling() {

}

void Culling::fixedUpdate() {

}

void Culling::update() {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f directrion = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;
	if (keyboard.keyDown(Keyboard::KEY_W)) {
		directrion += Vector3f(0.0f, 0.0f, 1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		directrion += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		directrion += Vector3f(-1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3f(1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		directrion += Vector3f(0.0f, -1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		directrion += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}

	Mouse &mouse = Mouse::instance();

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotate(dx, dy);

		}

		if (move) {
			m_camera.move(directrion * 25.0f * m_dt);
		}
	}
	m_trackball.idle();
	applyTransformation(m_trackball);
};

void Culling::render() {

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_quadTree.setFrustum(m_camera.getInvViewMatrix() * m_camera.getInvPerspectiveMatrix());

	int TrianglesRendered = m_quadTree.checkVisibility(true);

	auto shader = Globals::shaderManager.getAssetPointer("culling");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", Matrix4f::IDENTITY);
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix()));
	shader->loadVector("campos", !RenderTree2D ? m_camera.getPosition() : Vector3f(0.0f, 4096.0f, 0.0f));
	shader->loadVector("color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	m_terrain.drawRaw(m_quadTree);
	shader->unuse();

	renderUi();
}

void Culling::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void Culling::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);
	}
	else if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Culling::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);
	}
	else if (event.button == 2u) {
		Mouse::instance().detach();
	}
}


void Culling::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f * _180_ON_PI, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_trackball.reshape(Application::Width, Application::Height);
}

void Culling::applyTransformation(TrackBall& arc) {
	m_transform.fromMatrix(arc.getTransform());
}

void Culling::renderUi() {
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
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	ImGui::Checkbox("Draw AABB", &RenderAABB);
	ImGui::Checkbox("Draw Tree 2D", &RenderTree2D);
	ImGui::Checkbox("Draworder", &VisualizeRenderingOrder);
	ImGui::Checkbox("Sort Nodes", &SortVisibleGeometryNodes);
	ImGui::Checkbox("Performancetest", &VisibilityCheckingPerformanceTest);
	

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

