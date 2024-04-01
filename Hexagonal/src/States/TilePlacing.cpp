#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>

#include "TilePlacing.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"

unsigned int worldTest[16][16] = {
	{ 1,1,2,2,2,2,2,2,1,1,2,2,2,2,2,1 },
	{ 1,1,1,1,2,1,1,2,1,1,2,2,2,2,2,1 },
	{ 2,1,1,1,2,2,2,2,1,1,2,2,2,2,2,1 },
	{ 2,1,1,2,2,1,1,2,1,1,2,2,2,2,2,1 },
	{ 2,1,1,4,4,4,1,2,1,1,2,2,2,2,4,1 },
	{ 2,1,1,4,4,4,1,2,1,1,2,2,2,2,2,1 },
	{ 2,1,1,4,4,4,1,2,1,1,2,2,4,2,2,1 },
	{ 2,2,2,4,4,4,2,1,2,3,3,3,4,2,2,1 },
	{ 1,1,2,2,2,2,2,3,4,3,3,3,4,2,2,2 },
	{ 1,1,1,1,2,1,1,2,1,3,3,3,2,2,2,3 },
	{ 2,1,1,1,2,2,2,2,1,1,2,2,2,2,2,1 },
	{ 2,1,1,2,2,1,1,2,1,1,3,2,2,2,4,4 },
	{ 2,1,1,4,2,1,1,2,1,1,3,2,2,2,2,4 },
	{ 2,1,1,1,2,1,1,2,1,1,3,3,3,3,3,4 },
	{ 2,1,1,1,1,1,1,2,1,1,2,2,2,2,4,4 },
	{ 2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,1 }
};

TilePlacing::TilePlacing(StateMachine& machine) : State(machine, CurrentState::TILEPLACING), ISO_TILE_WIDTH(96), ISO_TILE_HEIGHT(48), ISO_CUBE_WIDTH(64), ISO_CUBE_HEIGHT(80), HEX_WIDTH(72), HEX_HEIGHT(46), HEX_OFFSET_X(54)/*out of gimp*/, HEX_OFFSET_Y(30)/*out of gimp*/ {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1000.0f, 1000.0f);
	//m_camera.orthographic(-static_cast<float>(Application::Width / 2), static_cast<float>(Application::Width / 2), -static_cast<float>(Application::Height / 2), static_cast<float>(Application::Height / 2), -1000.0f, 1000.0f);

	m_camera.lookAt(Vector3f(0.0f, -800.0f, 0.0f), Vector3f(0.0f, -800.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	//m_camera.lookAt(Vector3f(0.0f, 0.0f, 10.0f), Vector3f(0.0f, 0.0f, 10.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));	

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

	createBuffer(ISO_CUBE_WIDTH + 15, ISO_CUBE_HEIGHT);
}

TilePlacing::~TilePlacing() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void TilePlacing::fixedUpdate() {

}

void TilePlacing::update() {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f direction = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;
	if (keyboard.keyDown(Keyboard::KEY_W)) {
		direction += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		direction += Vector3f(0.0f, -1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		direction += Vector3f(-1.0f, 0.0f, 0.0f);
		m_background.addOffset(-0.001f);
		m_background.setSpeed(-0.005f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		direction += Vector3f(1.0f, 0.0f, 0.0f);
		m_background.addOffset(0.001f);
		m_background.setSpeed(0.005f);
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
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			//m_camera.rotate(dx, dy);

		}

		if (move) {
			m_camera.move(direction * m_dt);
		}
	}
	m_trackball.idle();
	m_transform.fromMatrix(m_trackball.getTransform());

	m_background.update(m_dt);
	//updateCamera(m_dt);

}

void TilePlacing::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//m_background.draw();
	if (renderMode != RenderMode::CPUCUBE) {
		glEnable(GL_BLEND);
		auto shader = Globals::shaderManager.getAssetPointer("batch");
		shader->use();
		shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * m_camera.getViewMatrix());

		int maxX = renderMode == RenderMode::ISOCUBE ? 16 : 40;
		int maxY = renderMode == RenderMode::ISOCUBE ? 16 : 40;

		for (int cx = 0; cx < maxX; ++cx) {
			for (int cy = 0; cy < maxY; ++cy) {
				float posX, posY;
				unsigned int id;
				int width, height;

				if (renderMode == RenderMode::ISOTILE) {
					Globals::spritesheetManager.getAssetPointer("isoTiles")->bind(0);
					id = m_tileId[cx][cy];
					width = ISO_TILE_WIDTH;
					height = ISO_TILE_HEIGHT;

					float pointX = cy * (float)(ISO_TILE_WIDTH) * 0.5f;
					float pointY = cx * (float)(ISO_TILE_WIDTH) * 0.5f;
					posX = (pointX - pointY) + Application::Width * 0.5f - (float)(ISO_TILE_WIDTH / 2);
					posY = (pointX + pointY) * 0.5f - (Application::Height *  0.5f - (float)(ISO_TILE_HEIGHT / 0.5f));

					Batchrenderer::Get().addQuadAA(Vector4f(posX, -posY, width, height), Vector4f(0.0f, 0.0f, 1.0f, 1.0f), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), id);

				}
				else if (renderMode == RenderMode::ISOHEX) {
					Globals::spritesheetManager.getAssetPointer("hexTiles")->bind(0);
					id = 1;
					width = HEX_WIDTH;
					height = HEX_HEIGHT;

					posX = (cx - cy) * HEX_OFFSET_X + Application::Width * 0.5f;
					posY = (cx + cy) * 0.5f * HEX_OFFSET_Y - Application::Height *  0.25f;

					Batchrenderer::Get().addQuadAA(Vector4f(posX, -posY, width, height), Vector4f(0.0f, 0.0f, 1.0f, 1.0f), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), id);

				}
				else if (renderMode == RenderMode::HEX) {
					Globals::spritesheetManager.getAssetPointer("hex")->bind(0);
					id = 0;
					width = HEX_WIDTH;
					height = HEX_WIDTH;

					float r = height * 0.5f;
					float h = width * 0.25f;
					float s = width * 0.5f;

					posX = cx * (h + s);
					posY = cy * height;

					if (cx % 2 == 0)
						posY -= r;

					Batchrenderer::Get().addQuadAA(Vector4f(posX, -posY, width, height), Vector4f(0.0f, 0.0f, 1.0f, 1.0f), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), id);

				}
				else if (renderMode == RenderMode::HEXFLIP) {
					Globals::spritesheetManager.getAssetPointer("hex")->bind(0);
					id = 1;
					width = HEX_WIDTH;
					height = HEX_WIDTH;

					float s = width * 0.5f;

					float h = height * 0.25f;
					float r = height * 0.5f;

					posX = cx * width;
					posY = cy * (h + r);

					if (cy % 2 == 0)
						posX += s;

					Batchrenderer::Get().addQuadAA(Vector4f(posX, -posY, width, height), Vector4f(0.0f, 0.0f, 1.0f, 1.0f), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), id);

				}
				else if (renderMode == RenderMode::CPUTILE) {
					Globals::spritesheetManager.getAssetPointer("tile")->bind(0);
					id = m_transparentTile ? 1u : 0u;

					//width = ISO_TILE_WIDTH;
					//height = ISO_TILE_HEIGHT;

					width = 128;
					height = 64;

					float pointX = cy * (float)(width) * 0.5f;
					float pointY = cx * (float)(width) * 0.5f;
					posX = (pointX - pointY) + Application::Width * 0.5f;
					posY = (pointX + pointY) * 0.5f - (Application::Height *  0.5f - 80.0f);

					Batchrenderer::Get().addDiamondAA(Vector4f(posX, -posY, width, height), Vector4f(0.0f, 0.0f, 1.0f, 1.0f), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), id);
				}
				else if (renderMode == RenderMode::ISOCUBE) {
					Globals::spritesheetManager.getAssetPointer("isoCubes")->bind(0);
					id = worldTest[cx][cy];
					width = ISO_CUBE_WIDTH;
					height = ISO_CUBE_HEIGHT;

					float pointX = cy * (float)(width) * 0.5f;
					float pointY = cx * (float)(width) * 0.5f;
					posX = (pointX - pointY) + Application::Width * 0.5f;
					posY = (pointX + pointY) * 0.5f;

					Batchrenderer::Get().addQuadAA(Vector4f(posX, -posY, width, height), Vector4f(0.0f, 0.0f, 1.0f, 1.0f), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), id);
				}
				else if (renderMode == RenderMode::CPUHEX) {
					Globals::spritesheetManager.getAssetPointer("hex")->bind(0);
					id = 0;
					width = 96;
					height = 48;

					float r = height * 0.5f;
					float h = width * 0.25f;
					float s = width * 0.5f;

					posX = cx * (h + s);
					posY = cy * height;

					if (cx % 2 == 0)
						posY -= r;

					Batchrenderer::Get().addHexagon(Vector4f(posX, -posY, width, height), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), id);
				}
				else if (renderMode == RenderMode::CPUHEXFLIP) {
					Globals::spritesheetManager.getAssetPointer("hex")->bind(0);
					id = 1;
					width = 96;
					height = 48;

					float s = width * 0.5f;

					float h = height * 0.25f;
					float r = height * 0.5f;

					posX = cx * width;
					posY = cy * (h + r);

					if (cy % 2 == 0)
						posX += s;

					Batchrenderer::Get().addHexagonFlip(Vector4f(posX, -posY, width, height), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), id);
				}
				else if (renderMode == RenderMode::CPUISOHEX) {
					//    Matrix4f rotX;
					//    rotX.rotate(Vector3f(1.0f, 0.0f, 0.0f), 60.0f);
					//    pos = rotX * pos <-> height = height /2 

					//    Matrix4f rotX;
					//    rotX.rotate(Vector3f(1.0f, 0.0f, 0.0f), 30.0f);
					//    pos = rotX * pos <-> height = height * sinf(60.0f) 

					Globals::spritesheetManager.getAssetPointer("hex")->bind(0);
					id = 0;

					int width = HEX_WIDTH;
					int height = HEX_WIDTH;
					float hex_offset_x = 54.0f;
					float hex_offset_y = 36.0f;

					float posX = (cx - cy) * hex_offset_x + Application::Width * 0.5f;
					float posY = (cx + cy) * 0.5f * hex_offset_y - Application::Height *  0.25f;

					Batchrenderer::Get().addHexagon(Vector4f(posX, -posY, width, height * 0.5f), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), id);
				}
				else if (renderMode == RenderMode::CPUISOHEXFLIP) {
					//    Matrix4f rotX;
					//    rotX.rotate(Vector3f(1.0f, 0.0f, 0.0f), 60.0f);
					//    pos = rotX * pos <-> height = height /2 

					//    Matrix4f rotX;
					//    rotX.rotate(Vector3f(1.0f, 0.0f, 0.0f), 30.0f);
					//    pos = rotX * pos <-> height = height * sinf(60.0f) 

					Globals::spritesheetManager.getAssetPointer("hex")->bind(0);
					id = 1;

					int width = HEX_WIDTH;
					int height = HEX_WIDTH;
					float hex_offset_x = 36.0f;
					float hex_offset_y = 54.0f;

					float posX = (cx - cy) * hex_offset_x + Application::Width * 0.5f;
					float posY = (cx + cy) * 0.5f * hex_offset_y - Application::Height *  0.25f;

					Batchrenderer::Get().addHexagonFlip(Vector4f(posX, -posY, width, height * 0.5f), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), id);
				}

			}
		}
		Batchrenderer::Get().drawBufferRaw();
		Spritesheet::Unbind();
		shader->unuse();
		glDisable(GL_BLEND);
	}
	else if (renderMode == RenderMode::CPUCUBE) {
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		auto shader = Globals::shaderManager.getAssetPointer("quad_array");
		shader->use();
		for (int cx = 0; cx < 20; ++cx) {
			for (int cy = 0; cy < 20; ++cy) {

				int width = ISO_CUBE_WIDTH;
				int height = ISO_CUBE_HEIGHT;

				float pointX = cy * (float)(width) * 0.5f;
				float pointY = cx * (float)(width) * 0.5f;
				float posX = (pointX - pointY) + Application::Width * 0.5f;
				float posY = (pointX + pointY) * 2.0f * 0.5f;

				shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * m_camera.getViewMatrix() * Matrix4f::Translate(posX, -posY, 0.0f));
				shader->loadInt("u_layer", m_transparentTile ? 1u : 0u);
				Globals::spritesheetManager.getAssetPointer("tile")->bind(0);

				glBindVertexArray(m_vao);
				glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
		}
		shader->unuse();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}

	if (m_drawUi)
		renderUi();
}

void TilePlacing::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void TilePlacing::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);
	}
	else if (event.button == 2u) {
		//m_drawUi = false;
		Mouse::instance().attach(Application::GetWindow());
	}
}

void TilePlacing::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);
	}
	else if (event.button == 2u) {
		//m_drawUi = true;
		Mouse::instance().detach();
	}
}

void TilePlacing::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void TilePlacing::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		ImGui::GetIO().WantCaptureMouse = false;
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void TilePlacing::OnKeyUp(Event::KeyboardEvent& event) {

}

void TilePlacing::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void TilePlacing::applyTransformation(TrackBall& arc) {
	m_transform.fromMatrix(arc.getTransform());
}

void TilePlacing::renderUi() {
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
	if (ImGui::Combo("Mode", &currentRenderMode, "Iso Tile\0Iso Hex\0Hex\0Hex Flip\0CPU Tile\0Iso Cube\0CPU Cube\0CPU Hex\0CPU Hex Flip\0CPU Iso Hex\0CPU Iso Hex Flip\0\0")) {
		renderMode = static_cast<RenderMode>(currentRenderMode);
	}

	if (renderMode == RenderMode::CPUTILE || renderMode == RenderMode::CPUCUBE) {

		//ImGui::SliderFloat("Scale", &m_scale, -10.0f, 10.0f);	
		ImGui::Checkbox("Transparent Tile", &m_transparentTile);
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void TilePlacing::createBuffer(float width, float height) {

	m_indexBuffer.push_back(0); m_indexBuffer.push_back(1); m_indexBuffer.push_back(2);
	m_indexBuffer.push_back(2); m_indexBuffer.push_back(3); m_indexBuffer.push_back(0);

	m_indexBuffer.push_back(4); m_indexBuffer.push_back(5); m_indexBuffer.push_back(6);
	m_indexBuffer.push_back(6); m_indexBuffer.push_back(7); m_indexBuffer.push_back(4);

	m_indexBuffer.push_back(8); m_indexBuffer.push_back(9); m_indexBuffer.push_back(10);
	m_indexBuffer.push_back(10); m_indexBuffer.push_back(11); m_indexBuffer.push_back(8);

	//m_indexBuffer.push_back(12); m_indexBuffer.push_back(13); m_indexBuffer.push_back(14);
	//m_indexBuffer.push_back(14); m_indexBuffer.push_back(15); m_indexBuffer.push_back(12);

	//m_indexBuffer.push_back(16); m_indexBuffer.push_back(17); m_indexBuffer.push_back(18);
	//m_indexBuffer.push_back(18); m_indexBuffer.push_back(19); m_indexBuffer.push_back(16);

	//m_indexBuffer.push_back(20); m_indexBuffer.push_back(21); m_indexBuffer.push_back(22);
	//m_indexBuffer.push_back(22); m_indexBuffer.push_back(23); m_indexBuffer.push_back(20);

	m_drawCount = m_indexBuffer.size();

	Vector3f pos0 = Vector3f(-0.5f * width, 0.5f * height, -0.5f * width);
	Vector3f pos1 = Vector3f(-0.5f * width, 0.5f * height, 0.5f * width);
	Vector3f pos2 = Vector3f(0.5f  * width, 0.5f * height, 0.5f * width);
	Vector3f pos3 = Vector3f(0.5f  * width, 0.5f * height, -0.5f * width);

	Vector3f pos4 = Vector3f(-0.5f * width, -0.5f * height, -0.5f * width);
	Vector3f pos5 = Vector3f(-0.5f * width, -0.5f * height, 0.5f * width);
	Vector3f pos6 = Vector3f(0.5f  * width, -0.5f * height, 0.5f * width);
	Vector3f pos7 = Vector3f(0.5f  * width, -0.5f * height, -0.5f * width);

	Matrix4f rotY;
	rotY.rotate(Vector3f(0.0f, 1.0f, 0.0f), 45.0f);

	Matrix4f rotX;
	rotX.rotate(Vector3f(1.0f, 0.0f, 0.0f), 30.0f);

	Matrix4f uniformScale;
	uniformScale.scale(M_SQRT1_3, M_SQRT1_3, M_SQRT1_3);

	m_positions.push_back(uniformScale * rotX * (rotY * (pos1)));
	m_positions.push_back(uniformScale * rotX * (rotY * (pos0)));
	m_positions.push_back(uniformScale * rotX * (rotY * (pos3)));
	m_positions.push_back(uniformScale * rotX * (rotY * (pos2)));

	m_positions.push_back(uniformScale * rotX * (rotY * (pos4)));
	m_positions.push_back(uniformScale * rotX * (rotY * (pos0)));
	m_positions.push_back(uniformScale * rotX * (rotY * (pos1)));
	m_positions.push_back(uniformScale * rotX * (rotY * (pos5)));

	m_positions.push_back(uniformScale * rotX * (rotY * (pos0)));
	m_positions.push_back(uniformScale * rotX * (rotY * (pos4)));
	m_positions.push_back(uniformScale * rotX * (rotY * (pos7)));
	m_positions.push_back(uniformScale * rotX * (rotY * (pos3)));


	m_positions.push_back(uniformScale * rotX * (rotY * (pos5)));
	m_positions.push_back(uniformScale * rotX * (rotY * (pos4)));
	m_positions.push_back(uniformScale * rotX * (rotY * (pos7)));
	m_positions.push_back(uniformScale * rotX * (rotY * (pos6)));

	m_positions.push_back(uniformScale * rotX * (rotY * (pos2)));
	m_positions.push_back(uniformScale * rotX * (rotY * (pos6)));
	m_positions.push_back(uniformScale * rotX * (rotY * (pos7)));
	m_positions.push_back(uniformScale * rotX * (rotY * (pos3)));

	m_positions.push_back(uniformScale * rotX * (rotY * (pos1)));
	m_positions.push_back(uniformScale * rotX * (rotY * (pos5)));
	m_positions.push_back(uniformScale * rotX * (rotY * (pos6)));
	m_positions.push_back(uniformScale * rotX * (rotY * (pos2)));

	m_texels.push_back(Vector2f(1.0f, 0.0f));
	m_texels.push_back(Vector2f(0.0f, 0.0f));
	m_texels.push_back(Vector2f(0.0f, 1.0f));
	m_texels.push_back(Vector2f(1.0f, 1.0f));

	m_texels.push_back(Vector2f(1.0f, 0.0f));
	m_texels.push_back(Vector2f(0.0f, 0.0f));
	m_texels.push_back(Vector2f(0.0f, 1.0f));
	m_texels.push_back(Vector2f(1.0f, 1.0f));

	m_texels.push_back(Vector2f(0.0f, 0.0f));
	m_texels.push_back(Vector2f(1.0f, 0.0f));
	m_texels.push_back(Vector2f(1.0f, 1.0f));
	m_texels.push_back(Vector2f(0.0f, 1.0f));


	m_texels.push_back(Vector2f(1.0f, 0.0f));
	m_texels.push_back(Vector2f(0.0f, 0.0f));
	m_texels.push_back(Vector2f(0.0f, 1.0f));
	m_texels.push_back(Vector2f(1.0f, 1.0f));

	m_texels.push_back(Vector2f(0.0f, 0.0f));
	m_texels.push_back(Vector2f(1.0f, 0.0f));
	m_texels.push_back(Vector2f(1.0f, 1.0f));
	m_texels.push_back(Vector2f(0.0f, 1.0f));

	m_texels.push_back(Vector2f(0.0f, 0.0f));
	m_texels.push_back(Vector2f(1.0f, 0.0f));
	m_texels.push_back(Vector2f(1.0f, 1.0f));
	m_texels.push_back(Vector2f(0.0f, 1.0f));

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(m_numBuffers, m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	//Position
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(m_positions[0]), &m_positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, m_texels.size() * sizeof(m_texels[0]), &m_texels[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(m_indexBuffer[0]), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &ibo);
}