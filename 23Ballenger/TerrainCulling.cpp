#include <time.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "TerrainCulling.h"
#include "Application.h"
#include "Menu.h"
#include "Globals.h"

TerrainCulling::TerrainCulling(StateMachine& machine) : State(machine, CurrentState::TERRAINCULLING) {

	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 1000.0f);
	
	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(-2.5f);
	applyTransformation(m_trackball);

	m_terrain.init("Levels/terrain01.raw");

	m_quadTree.init(m_terrain.getPositions().data(), m_terrain.getIndexBuffer().data(), m_terrain.getIndexBuffer().size(), m_terrain.getMin(), m_terrain.getMax(), 64.0f);
	Globals::shaderManager.loadShader("culling", "res/terrain.vert", "res/terrain.frag");

	float height = m_terrain.heightAt(512.0f, 512.0f);
	m_camera.lookAt(Vector3f(512.0f, height + 1.75f, 512.0f), Vector3f(512.0f, height + 1.75f, 512.0f -1.0f), Vector3f(0.0f, 1.0f, 0.0f));

	float offset = 20.0f;
	m_view.lookAt(Vector3f(512.0f, 0.0f, 512.0f), Vector3f(512.0f, 0.0f - 1.0f, 512.0f), Vector3f(0.0f, 0.0f, -1.0f));
	m_orthographic.orthographic(-(512.0f + offset), 512.0f + offset, -(512.0f + offset), 512.0f + offset, -4096.0f, 4096.0f);
	m_perspective.perspective(m_fovx, (float)Application::Width / (float)Application::Height, m_near, m_far);
}

TerrainCulling::~TerrainCulling() {
	EventDispatcher::RemoveMouseListener(this);
	EventDispatcher::RemoveKeyboardListener(this);
}

void TerrainCulling::fixedUpdate() {

}

void TerrainCulling::update() {
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
	m_perspective.perspective(m_fovx, (float)Application::Width / (float)Application::Height, m_near, m_far);
};

void TerrainCulling::render() {

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("culling");
	shader->use();
	shader->loadMatrix("u_projection", !m_overview ? m_camera.getPerspectiveMatrix() : m_orthographic);
	shader->loadMatrix("u_view", !m_overview ? m_camera.getViewMatrix() : m_view);
	shader->loadMatrix("u_model", Matrix4f::IDENTITY);
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix()));
	shader->loadVector("campos", !m_overview ? m_camera.getPosition() : Vector3f(512.0f, 4096.0f, 512.0f));
	shader->loadVector("color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

	if (m_useCulling) {
		QuadTree::Frustum.updatePlane(m_perspective, m_camera.getViewMatrix());
		int TrianglesRendered = m_quadTree.checkVisibility(m_sortVisibility);

		m_terrain.bindVAO();
		m_quadTree.draw(m_showDrawOrder);
		m_terrain.unbindVAO();
		
	} else {
		m_terrain.drawRaw();
	}	
	shader->unuse();

	if (m_drawAABB){
		//exceptionally use the fixed pipeline here to avoid creating of unwanted vaos for every node
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glLoadMatrixf(!m_overview ? &m_camera.getPerspectiveMatrix()[0][0] : &m_orthographic[0][0]);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLoadMatrixf(!m_overview ? &m_camera.getViewMatrix()[0][0] : &m_view[0][0]);

		glColor3f(0.0f, 1.0f, 0.0f);
		m_quadTree.drawAABB(m_depth);
	}

	QuadTree::Frustum.update(m_perspective, m_camera.getViewMatrix());
	!m_overview ? QuadTree::Frustum.drawFrustm(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix(), m_distance) : QuadTree::Frustum.drawFrustm(m_orthographic, m_view, m_distance);
	renderUi();
}

void TerrainCulling::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void TerrainCulling::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);
	}
	else if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void TerrainCulling::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);
	}
	else if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void TerrainCulling::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		ImGui::GetIO().WantCaptureMouse = false;
		Mouse::instance().detach();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void TerrainCulling::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f * _180_ON_PI, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_trackball.reshape(Application::Width, Application::Height);
}

void TerrainCulling::applyTransformation(TrackBall& arc) {
	m_transform.fromMatrix(arc.getTransform());
}

void TerrainCulling::renderUi() {
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
	if (ImGui::Button("Toggle VSYNC")){
		Application::ToggleVerticalSync();
	}
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	ImGui::Checkbox("Overview", &m_overview);
	ImGui::Checkbox("Draw AABB", &m_drawAABB);
	ImGui::SliderInt("Depth", &m_depth, -1, 20);
	ImGui::Checkbox("Show Draw Order", &m_showDrawOrder);
	ImGui::Checkbox("Sort Nodes", &m_sortVisibility);
	ImGui::Checkbox("Use Culling", &m_useCulling);
	ImGui::Checkbox("Show Frustum", &QuadTree::Frustum.getDebug());

	ImGui::SliderFloat("Fovx", &m_fovx, 0.01f, 180.0f);
	ImGui::SliderFloat("Far", &m_far, 25.0f, 1100.0f);
	ImGui::SliderFloat("Near", &m_near, 1.0f, 200.0f);
	ImGui::SliderFloat("Distance", &m_distance, -100.0f, 100.0f);

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

