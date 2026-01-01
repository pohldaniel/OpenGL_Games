#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <States/Menu.h>
#include <Entities/SpriteEntity.h>
#include <Entities/Player.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"

Game::Game(StateMachine& machine) : State(machine, States::GAME), m_debugCollision(true){

	m_viewWidth = 1280.0f;
	m_viewHeight = 720.0f;
	m_movingSpeed = 350.0f;
	m_screeBorder = 0.0f;

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.orthographic(0.0f, m_viewWidth, 0.0f, m_viewHeight, -1.0f, 1.0f);
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(m_movingSpeed);
	m_scene == Scene::OMNI ? m_camera.setPosition(0.0f, 240.0f, 0.0f) : m_camera.setPosition(400.0f, 400.0f, 0.0f);

	m_scene == Scene::OMNI && m_debugCollision ? glClearColor(0.0f, 0.0f, 0.0f, 1.0f) : glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
	glClearDepth(1.0f);

	auto shader = Globals::shaderManager.getAssetPointer("batch");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix());
	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("font_ttf");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix());
	shader->unuse();

	Zone::LoadTileSetData("res/tilesets.json");

	m_zone = new Level(m_camera);
	m_zone->loadZone("res/data/levels/omni.tmx", "omni");
	m_zone->resize();
	m_zone->setDebugCollision(m_debugCollision);

	Level::GetPlayer().setMovingSpeed(m_movingSpeed);
	Level::GetPlayer().setViewWidth(m_viewWidth);
	Level::GetPlayer().setViewHeight(m_viewHeight);
}

Game::~Game() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	delete m_zone;
}

void Game::fixedUpdate() {

}

void Game::update() {
	
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

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		direction += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		direction += Vector3f(0.0f, -1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		m_scene == Scene::OMNI ? m_scene = Scene::OVERWORLD : m_scene = Scene::OMNI;
		m_scene == Scene::OMNI ? m_camera.setPosition(0.0f, 240.0f, 0.0f) : m_camera.setPosition(400.0f, 400.0f, 0.0f);
		m_scene == Scene::OMNI && m_debugCollision ? glClearColor(0.0f, 0.0f, 0.0f, 1.0f) : glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
	}

	if (m_scene == Scene::OVERWORLD) {
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
	}
	m_zone->update(m_dt);
}

void Game::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_zone->draw();

#if DEVBUILD
	if (m_drawUi)
		renderUi();
#endif
}

void Game::OnReEnter(unsigned int prevState) {

}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Game::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}

	if (event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void Game::OnKeyDown(Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		auto shader = Globals::shaderManager.getAssetPointer("batch");
		shader->use();
		shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f));
		shader->unuse();

		shader = Globals::shaderManager.getAssetPointer("font_ttf");
		shader->use();
		shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f));
		shader->unuse();

		Mouse::instance().detach();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Game::OnKeyUp(Event::KeyboardEvent& event) {

}

void Game::resize(int deltaW, int deltaH) {
	m_camera.orthographic(0.0f, m_viewWidth, 0.0f, m_viewHeight, -1.0f, 1.0f);
	m_zone->resize();

	auto shader = Globals::shaderManager.getAssetPointer("batch");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix());
	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("font_ttf");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix());
	shader->unuse();
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
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Left, 0.22f, nullptr, &dockSpaceId);
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Right, 0.22f, nullptr, &dockSpaceId);
		ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Down, 0.22f, nullptr, &dockSpaceId);
		ImGuiID dock_id_up = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Up, 0.22f, nullptr, &dockSpaceId);
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());

	if (m_scene == Scene::OVERWORLD && ImGui::Checkbox("Draw Center", &m_drawCenter)) {
		m_zone->setDrawCenter(m_drawCenter);
	}
	

	if (ImGui::Checkbox("Use Culling", &m_useCulling)) {
		m_zone->setUseCulling(m_useCulling);
	}

	if (ImGui::Checkbox("Debug Collision", &m_debugCollision)) {
		m_zone->setDebugCollision(m_debugCollision);
		m_debugCollision && m_scene == Scene::OMNI ? glClearColor(0.0f, 0.0f, 0.0f, 1.0f) : glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
	}

	int currentScene = m_scene;
	if (ImGui::Combo("Scene", &currentScene, "Overworld\0Omni\0\0")) {
		m_scene = static_cast<Scene>(currentScene);		
		if (m_scene == Scene::OVERWORLD) {
			delete m_zone;
			m_zone = new Overworld(m_camera);
			m_zone->loadZone("res/data/overworld/overworld.tmx", "overworld");
			m_zone->resize();
			m_zone->setDebugCollision(m_debugCollision);


			m_camera.setPosition(400.0f, 400.0f, 0.0f);
			glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
		}

		if (m_scene == Scene::OMNI) {
			delete m_zone;
			m_zone = new Level(m_camera);
			m_zone->loadZone("res/data/levels/omni.tmx", "omni");
			m_zone->resize();
			m_zone->setDebugCollision(m_debugCollision);
			
			Level::GetPlayer().reset();
			Level::GetPlayer().setMovingSpeed(m_movingSpeed);
			Level::GetPlayer().setViewWidth(m_viewWidth);
			Level::GetPlayer().setViewHeight(m_viewHeight);

			m_camera.setPosition(0.0f, 240.0f, 0.0f);
			m_debugCollision ? glClearColor(0.0f, 0.0f, 0.0f, 1.0f) : glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
			m_drawCenter = false;
		}
	}

	if (m_scene == Scene::OMNI && ImGui::Button("Reset player")) {
		Level::GetPlayer().reset();
	}

	if (m_scene == Scene::OMNI && ImGui::SliderFloat("Size X", &Level::GetPlayer().sizeX(), 0.0f, 48.0f)) {

	}
	
	if (m_scene == Scene::OMNI && ImGui::SliderFloat("Size Y", &Level::GetPlayer().sizeY(), 0.0f, 56.0f)) {

	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}