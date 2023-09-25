#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), ISO_WIDTH(96), ISO_HEIGHT(48), HEX_WIDTH(72), HEX_HEIGHT(46), HEX_OFFSET_X(54)/*out of gimp*/, HEX_OFFSET_Y(30)/*out of gimp*/ {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, -800.0f, 0.0f), Vector3f(0.0f, -800.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(400.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	m_background.setLayer(std::vector<BackgroundLayer>{
		{ &Globals::textureManager.get("forest_1"), 1, 1.0f },
		{ &Globals::textureManager.get("forest_2"), 1, 2.0f },
		{ &Globals::textureManager.get("forest_3"), 1, 3.0f },
		{ &Globals::textureManager.get("forest_4"), 1, 4.0f },
		{ &Globals::textureManager.get("forest_5"), 1, 5.0f }});
	m_background.setSpeed(0.005f);

	std::fstream fs("res/maps/40x40-01.map");
	if (!fs.is_open())
		return;

	std::string line;
	std::istringstream ss;

	// reading map size
	std::getline(fs, line);
	ss.str(line);

	unsigned int rows = 0;
	unsigned int cols = 0;
	ss >> rows >> cols;

	m_cols = cols;
	m_rows = rows;

	m_tileId = new unsigned int*[m_rows];
	for (int i = 0; i < m_rows; ++i)
		m_tileId[i] = new unsigned int[m_cols];

	for (int r = 0; r < rows; ++r) {

		std::getline(fs, line);
		ss.clear();
		ss.str(line);
		for (int c = 0; c < cols; ++c) {
			ss >> m_tileId[r][c];
		
		}
	}
	fs.close();
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
		directrion += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		directrion += Vector3f(0.0f, -1.0f, 0.0f);
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
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			//m_camera.rotate(dx, dy);

		}

		if (move) {
			m_camera.move(directrion * m_dt);
		}
	}
	m_trackball.idle();
	m_transform.fromMatrix(m_trackball.getTransform());

	m_background.update(m_dt);
	//updateCamera(m_dt);

}

void Game::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//m_background.draw();

	glEnable(GL_BLEND);
	auto shader = Globals::shaderManager.getAssetPointer("batch");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * m_camera.getViewMatrix());
	

	for (int cx = 0; cx < 40; ++cx) {
		for (int cy = 0; cy < 40; ++cy) {
			float posX, posY;
			unsigned int id;
			int width, height;

			if (renderMode == RenderMode::ISOTILE) {
				Globals::spritesheetManager.getAssetPointer("isoTiles")->bind(0);
				id = m_tileId[cx][cy];
				width = ISO_WIDTH;
				height = ISO_HEIGHT;

				float pointX = cy * (float)(ISO_WIDTH) * 0.5f;
				float pointY = cx * (float)(ISO_WIDTH) * 0.5f;
				posX = (pointX - pointY) + Application::Width * 0.5f - (float)(ISO_WIDTH / 2);
				posY = (pointX + pointY) * 0.5f - (Application::Height *  0.5f - (float)(ISO_HEIGHT / 0.5f));

			}else if (renderMode == RenderMode::ISOHEX) {
				Globals::spritesheetManager.getAssetPointer("hexTiles")->bind(0);
				id = 1;
				width = HEX_WIDTH;
				height = HEX_HEIGHT;

				posX = (cx - cy) * HEX_OFFSET_X + Application::Width * 0.5f - (float)(ISO_WIDTH / 2);
				posY = (cx + cy) * 0.5f * HEX_OFFSET_Y - (Application::Height *  0.25f - (float)(ISO_HEIGHT / 0.5f));

			}else if (renderMode == RenderMode::HEX) {
				Globals::spritesheetManager.getAssetPointer("hex")->bind(0);
				id = 0;
				width = HEX_WIDTH;
				height = HEX_WIDTH;

				float h = HEX_WIDTH * 0.25f;
				float s = HEX_WIDTH * 0.5f;
				float r = HEX_WIDTH * 0.5f;

				posX = cx * (h + s);
				posY = cy * HEX_WIDTH;

				if (cx % 2 == 0)
					posY -= r;

			}else if (renderMode == RenderMode::HEXFLIP) {
				Globals::spritesheetManager.getAssetPointer("hex_flip")->bind(0);
				id = 0;
				width = HEX_WIDTH;
				height = HEX_WIDTH;

				float h = HEX_WIDTH * 0.25f;
				float s = HEX_WIDTH * 0.5f;
				float r = HEX_WIDTH * 0.5f;

				posX = cx * HEX_WIDTH;
				posY = cy * (h + s);

				if (cy % 2 == 0)
					posX += s;

			}
			Batchrenderer::Get().addQuadAA(Vector4f(posX, -posY, width, height), Vector4f(0.0f, 0.0f, 1.0f, 1.0f), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), id);
		}
	}	
	Batchrenderer::Get().drawBufferRaw();
	Globals::spritesheetManager.getAssetPointer("isoTiles")->unbind(0);
	shader->unuse();
	glDisable(GL_BLEND);


	/*glEnable(GL_BLEND);
	auto shader = Globals::shaderManager.getAssetPointer("batch");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * m_camera.getViewMatrix());
	Globals::spritesheetManager.getAssetPointer("hexTiles")->bind(0);

	for (int r = 0; r < 40; ++r) {
		for (int c = 0; c < 40; ++c) {

			float pointXTrans = (r - c) * HEX_OFFSET_X + Application::Width * 0.5f - (float)(ISO_WIDTH / 2);
			float pointYTrans = (r + c) * 0.5f * HEX_OFFSET_Y - (Application::Height *  0.25f - (float)(ISO_HEIGHT / 0.5f));

			
			Batchrenderer::Get().addQuadAA(Vector4f(pointXTrans, -pointYTrans, HEX_WIDTH, HEX_HEIGHT), Vector4f(0.0f, 0.0f, 1.0f, 1.0f), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1);
		}
	}
	Batchrenderer::Get().drawBufferRaw();
	Globals::spritesheetManager.getAssetPointer("hexTiles")->unbind(0);
	shader->unuse();
	glDisable(GL_BLEND);*/

	if (m_drawUi)
		renderUi();

}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {		
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		//m_drawUi = false;
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {		
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		//m_drawUi = true;
		Mouse::instance().detach();
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
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());

	int currentRenderMode = renderMode;
	if (ImGui::Combo("Mode", &currentRenderMode, "Iso Tile\0Iso Hex\0Hex\0Hex Flip\0\0")) {
		renderMode = static_cast<RenderMode>(currentRenderMode);
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}