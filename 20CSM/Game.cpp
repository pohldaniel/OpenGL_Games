#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "engine/input/EventDispatcher.h"

#include "Game.h"
#include "Application.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0, (double)Application::Width / (double)Application::Height, 0.1f, 1000.0f);
	m_camera.lookAt(Vector3f(75.5f, 30.0f, -110.0f), Vector3f(75.5f - 0.7f, 30.0f, -110.0f + 0.7f), Vector3f(0.0f, 1.0f, 0.0f));

	m_trackball.reshape(Application::Width, Application::Height);
	applyTransformation(m_trackball);

	terrain = new TerrainNV();
	terrain->Load();

	int td = terrain->getDim() / 2;
	obj_BSphere[0].center = Vector3f(-td, 50.0, -td);
	obj_BSphere[1].center = Vector3f(-td, 50.0, td);
	obj_BSphere[2].center = Vector3f(td, 50.0, td);
	obj_BSphere[3].center = Vector3f(td, 50.0, -td);
	obj_BSphere[0].radius = 1.0f;
	obj_BSphere[1].radius = 1.0f;
	obj_BSphere[2].radius = 1.0f;
	obj_BSphere[3].radius = 1.0f;

	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
}

Game::~Game() {}

void Game::fixedUpdate() {

}

void Game::update() {
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
			m_camera.rotateSmoothly(dx, dy, 0.0f);

		}

		if (move) {
			m_camera.move(directrion * 20.0f * m_dt);
		}
	}

	m_trackball.idle();
	applyTransformation(m_trackball);
};

void Game::render(unsigned int &frameBuffer) {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&m_camera.getProjectionMatrix()[0][0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(&m_camera.getViewMatrix()[0][0]);

	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderScene();
	//renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
	applyTransformation(m_trackball);
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
	applyTransformation(m_trackball);
}

void Game::applyTransformation(TrackBall& arc) {

	m_transform.fromMatrix(arc.getTransform(Vector3f(0.0f, 1.0f, 0.0f), 180.0f));
}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(40.0, (double)Application::Width / (double)Application::Height, 1.0, 100.0);
	m_trackball.reshape(Application::Width, Application::Height);
}

void Game::renderScene() {	
	terrain->Draw(m_camera);
}

void Game::renderUi() {
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
	ImGui::Checkbox("Draw Wirframe", &Globals::enableWireframe);
	
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}