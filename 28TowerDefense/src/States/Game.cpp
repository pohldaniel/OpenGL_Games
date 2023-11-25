#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"

Game::Game(StateMachine& machine) : State(machine, States::GAME), m_level(m_camera, static_cast<float>(Application::Width) / 48.0f, static_cast<float>(Application::Height) / 48.0f) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 2.0f, 10.0f), Vector3f(0.0f, 2.0f, 10.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(-5.0f);
	applyTransformation(m_trackball);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	m_background.setLayer(std::vector<BackgroundLayer>{
		{ &Globals::textureManager.get("forest_1"), 1, 1.0f },
		{ &Globals::textureManager.get("forest_2"), 1, 2.0f },
		{ &Globals::textureManager.get("forest_3"), 1, 3.0f },
		{ &Globals::textureManager.get("forest_4"), 1, 4.0f },
		{ &Globals::textureManager.get("forest_5"), 1, 5.0f }});
	m_background.setSpeed(0.005f);
}

Game::~Game() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

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
		m_background.addOffset(-0.001f);
		m_background.setSpeed(-0.005f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3f(1.0f, 0.0f, 0.0f);
		m_background.addOffset(0.001f);
		m_background.setSpeed(0.005f);
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
		dx = mouse.xDelta();
		dy = mouse.yDelta();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotate(dx, dy);
		}

		if (move) {
			m_camera.move(directrion * m_dt);
		}
	}

	Vector2f posMouse((float)m_mouseX / tileSize, (float)(Application::Height - m_mouseY) / tileSize);
	m_level.setTargetAndCalculateFlowField((int)posMouse[0], (int)posMouse[1]);



	if (m_mouseDownLeft || m_mouseDownRight) {
		if (m_mouseDownRight) {
			m_level.setTileWall((int)posMouse[0], (int)posMouse[1], false);
			removeUnitsAtMousePosition(posMouse);
		}

		if (m_mouseDownLeft) {
			if (keyboard.keyDown(Keyboard::KEY_CTRL)) {
				addUnit(posMouse);
			}else {
				m_level.setTileWall((int)posMouse[0], (int)posMouse[1], true);
			}
			
		}
	}

	m_background.update(m_dt);
	m_trackball.idle();
	m_transform.fromMatrix(m_trackball.getTransform());

	for (auto& unitSelected : listUnits)
		unitSelected.update(m_dt, m_level, listUnits);
}

void Game::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//m_background.draw();
	
	m_level.draw(tileSize);

	for (auto& unitSelected : listUnits)
		unitSelected.drawBatched(tileSize);

	Batchrenderer::Get().drawBuffer();

	//if (m_drawUi)
		//renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {	
	m_mouseX = event.x;
	m_mouseY = event.y;

	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
} 

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_mouseDownLeft = event.button == 1u;
	m_mouseDownRight = event.button == 2u;
	m_mouseX = event.x;
	m_mouseY = event.y;

	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		//Mouse::instance().attach(Application::GetWindow());
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	m_mouseDownLeft = false;
	m_mouseDownRight = false;

	if (event.button == 1u) {		
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		//Mouse::instance().detach();
	}
}

void Game::OnMouseWheel(Event::MouseWheelEvent& event) {
	
}

void Game::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		ImGui::GetIO().WantCaptureMouse = false;
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void Game::OnKeyUp(Event::KeyboardEvent& event) {

}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void Game::applyTransformation(TrackBall& arc) {
	m_transform.fromMatrix(arc.getTransform());
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

void Game::addUnit(const Vector2f& posMouse) {
	listUnits.push_back(Unit(posMouse));
}



void Game::removeUnitsAtMousePosition(const Vector2f& posMouse) {
	for (int count = 0; count < listUnits.size(); count++) {
		auto& unitSelected = listUnits[count];
		if (unitSelected.checkOverlap(posMouse, 0.0f)) {
			listUnits.erase(listUnits.begin() + count);
			count--;
		}
	}
}