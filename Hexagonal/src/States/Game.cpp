#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"
#include "Utils/Rasterizer.h"

#define MAX_ESTRING_LENGTH 128

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(-800.0f, -1500.0f, 0.0f), Vector3f(-800.0f, -1500.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));


	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(400.0f);

	m_left = m_camera.getLeftOrthographic();
	m_right = m_camera.getRightOrthographic();
	m_bottom = m_camera.getBottomOrthographic();
	m_top = m_camera.getTopOrthographic();

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	m_background.setLayer(std::vector<BackgroundLayer>{
		{ &Globals::textureManager.get("forest_1"), 1, 1.0f },
		{ &Globals::textureManager.get("forest_2"), 1, 2.0f },
		{ &Globals::textureManager.get("forest_3"), 1, 3.0f },
		{ &Globals::textureManager.get("forest_4"), 1, 4.0f },
		{ &Globals::textureManager.get("forest_5"), 1, 5.0f }});
	m_background.setSpeed(0.005f);

	m_mainRT.create(Application::Width, Application::Height);
	m_mainRT.attachTexture2D(AttachmentTex::RGBA);
	m_mainRT.attachRenderbuffer(AttachmentRB::DEPTH24);

	
	TileSetManager::Get().getTileSet("map").loadTileSet("res/tilesetFrames2.bimg");
	m_atlas = TileSetManager::Get().getTileSet("map").getAtlas();
	//Spritesheet::Safe("map", m_atlas);

	loadMap("res/EvilTown2.emap");

	m_focusPointX = static_cast<float>(Application::Width / 2);
	m_focusPointY = static_cast<float>(Application::Height / 2);

	TileSetManager::Get().getTileSet("sHero_anm").loadTileSet("Graphics/Animations/sHero/Controller_defs/sHero.bimg", 1280u, 1280u, true);
	//Spritesheet::Safe("sHero", TileSetManager::Get().getTileSet("sHero_anm").getAtlas());

	TileSetManager::Get().getTileSet("sArcher_anm").loadTileSet("Graphics/Animations/sArcher/Controller_defs/sArcher.bimg", 1280u, 1024u, true);
	//Spritesheet::Safe("sArcher", TileSetManager::Get().getTileSet("sArcher_anm").getAtlas());

	loadAnimation("Graphics/Animations/sHero/Controller_defs/sHero_run.banim");
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
	move = false;

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		directrion += Vector3f(0.0f, 1.0f * (1.0f / m_zoomFactor), 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		directrion += Vector3f(0.0f, -1.0f * (1.0f / m_zoomFactor), 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		directrion += Vector3f(-1.0f * (1.0f / m_zoomFactor), 0.0f, 0.0f);
		m_background.addOffset(-0.001f);
		m_background.setSpeed(-0.005f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3f(1.0f * (1.0f / m_zoomFactor), 0.0f, 0.0f);
		m_background.addOffset(0.001f);
		m_background.setSpeed(0.005f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		directrion += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		directrion += Vector3f(0.0f, 0.0f, 1.0f);
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
}

void Game::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	auto shader = Globals::shaderManager.getAssetPointer("quad_array");
	shader->use();

	const TextureRect& rect = m_animationFrames[m_direction * 14 + m_animationFrame].rect;
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * Matrix4f::Translate(800.0f, 450.0f, 0.0f) * Matrix4f::Scale(rect.width, rect.height, 0.0f));
	shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
	shader->loadInt("u_layer", rect.frame);
	Spritesheet::Bind(TileSetManager::Get().getTileSet("sHero_anm").getAtlas());

	Globals::shapeManager.get("quad").drawRaw();

	Spritesheet::Unbind();
	shader->unuse();
	glDisable(GL_BLEND);

	/*if (m_redrawMap) {
		m_redrawMap = !m_autoRedraw;
		m_mainRT.bindWrite();
		culling();
		glClearBufferfv(GL_COLOR, 0, std::vector<float>{0.494f, 0.686f, 0.796f, 1.0f}.data());
		glClearBufferfv(GL_DEPTH, 0, &std::vector<float>{1.0f}[0]);
		//m_background.draw();
		glPolygonMode(GL_FRONT_AND_BACK, StateMachine::GetEnableWireframe() ? GL_LINE : GL_FILL);
		glEnable(GL_BLEND);
		auto shader = Globals::shaderManager.getAssetPointer("batch");
		shader->use();
		shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * m_camera.getViewMatrix());

		Spritesheet::Bind(m_atlas);
		for (auto cell : m_useCulling ? m_visibleCells : m_cells) {
			if (cell.visible)
				Batchrenderer::Get().addQuadAA(Vector4f(cell.posX * m_zoomFactor + (m_camera.getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor), cell.posY * m_zoomFactor + (m_camera.getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor), cell.rect.width * m_zoomFactor, cell.rect.height * m_zoomFactor), Vector4f(cell.rect.textureOffsetX, cell.rect.textureOffsetY, cell.rect.textureWidth, cell.rect.textureHeight), cell.selected ? Vector4f(0.5f, 0.5f, 0.5f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f), cell.rect.frame);
		}

		Batchrenderer::Get().drawBufferRaw();
		Spritesheet::Unbind();
		shader->unuse();
		glDisable(GL_BLEND);

		if (m_drawCullingRect)
			drawCullingRect();

		drawMouseRect();
		m_mainRT.unbindWrite();
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	shader->loadBool("u_flip", false);
	m_mainRT.bindColorTexture();
	m_zoomableQuad.drawRaw();
	m_mainRT.unbindColorTexture();
	shader->unuse();*/

	if (m_drawUi)
		renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);

	if (m_mouseDown) {
		m_mouseMove = true;
		for (auto cell : m_singleCache) {
			cell.found = false;
		}

		if(m_selectionMode != SelectionMode::MARKER)
			processCache(m_cellCache, true, false, true);

		float offsetX = m_zoomFactor * (m_camera.getPositionX() + m_focusPointX) - m_focusPointX;
		float offsetY = m_zoomFactor * (m_camera.getPositionY() + m_focusPointY) - m_focusPointY;

		float mouseViewX = static_cast<float>(event.x);
		float mouseViewY = static_cast<float>(Application::Height - event.y);

		m_curMouseX = mouseViewX;
		m_curMouseY = mouseViewY;
		if(m_selectionMode == SelectionMode::RASTERIZER) {
			std::vector<std::array<int, 2>> points;
			points.clear();

			//int startX, startY;
			//isometricToCartesian(m_mouseX + offsetX, m_mouseY + offsetY, startX, startY, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);

			//int endX, endY;
			//isometricToCartesian(mouseViewX + offsetX, mouseViewY + offsetY, endX, endY, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);

			//Rasterizer::supercover_line( startX, startY ,  endX, endY , points);
			//Rasterizer::linev5(startX, startY, endX, endY, points);

			float left = std::min(m_mouseX, mouseViewX);
			float right = std::max(m_mouseX, mouseViewX);
			float bottom = std::min(m_mouseY, mouseViewY);
			float top = std::max(m_mouseY, mouseViewY);

			std::array<Vector2f, 4> corners;

			corners[0] = Vector2f(left, bottom);
			corners[1] = Vector2f(left, top);
			corners[2] = Vector2f(right, top);
			corners[3] = Vector2f(right, bottom);

			int point01, point02;
			isometricToCartesian(corners[0][0] + offsetX, corners[0][1] + offsetY, point01, point02, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);

			int point11, point12;
			isometricToCartesian(corners[1][0] + offsetX, corners[1][1] + offsetY, point11, point12, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);

			int point21, point22;
			isometricToCartesian(corners[2][0] + offsetX, corners[2][1] + offsetY, point21, point22, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);

			int point31, point32;
			isometricToCartesian(corners[3][0] + offsetX, corners[3][1] + offsetY, point31, point32, cellWidth * m_zoomFactor, cellHeight * m_zoomFactor);


			std::vector<std::array<int, 2>> initial;
			initial.push_back({ point01, point02 });
			initial.push_back({ point11, point12 });
			initial.push_back({ point31, point32 });

			Rasterizer::rasterTriangle(initial, points);

			std::vector<std::array<int, 2>> initial2;
			initial2.push_back({ point11, point12 });
			initial2.push_back({ point21, point22 });
			initial2.push_back({ point31, point32 });

			Rasterizer::rasterTriangle(initial2, points);

			for (auto point : points) {
				for (int j = 0; j < m_layer.size(); j++) {
					if (isValid(point[0], point[1]) && m_layer[j][point[1]][point[0]].first != -1) {
						if (!m_cells[m_layer[j][point[1]][point[0]].second].selected && m_cells[m_layer[j][point[1]][point[0]].second].visible) {
							m_cellCache.push_back(m_cells[m_layer[j][point[1]][point[0]].second]);
							m_cellCache.back().get().selected = true;
						}
						SingleSelectedCell defaultCell = { point[0], point[1], false };
						std::vector<SingleSelectedCell>::iterator it = std::find_if(m_singleCache.begin(), m_singleCache.end(), std::bind(FindSingleCell, std::placeholders::_1, defaultCell));
						if (it != m_singleCache.end())
							it->found = true;
					}
				}
			}


		}else if(m_selectionMode == SelectionMode::BOXSELECTION) {
			//box selection
			int left = static_cast<int>(std::min(m_mouseX, mouseViewX));
			int right = static_cast<int>(std::max(m_mouseX, mouseViewX));
			int bottom = static_cast<int>(std::min(m_mouseY, mouseViewY));
			int top = static_cast<int>(std::max(m_mouseY, mouseViewY));

			for (int x = left; x < right; x = x + cellWidth * m_zoomFactor) {
				for (int y = bottom; y < top; y = y + cellHeight * 0.5f * m_zoomFactor) {
					int row, col;
					isometricToCartesian(x + offsetX, y + offsetY, row, col, cellWidth  * m_zoomFactor, cellHeight  * m_zoomFactor);
					for (int j = 0; j < m_layer.size(); j++) {
						if (isValid(row, col) && m_layer[j][col][row].first != -1) {
							if (!m_cells[m_layer[j][col][row].second].selected && m_cells[m_layer[j][col][row].second].visible) {
								m_cellCache.push_back(m_cells[m_layer[j][col][row].second]);
								m_cellCache.back().get().selected = true;
							}
							SingleSelectedCell defaultCell = { row, col, false };
							std::vector<SingleSelectedCell>::iterator it = std::find_if(m_singleCache.begin(), m_singleCache.end(), std::bind(FindSingleCell, std::placeholders::_1, defaultCell));
							if (it != m_singleCache.end())
								it->found = true;
						}
					}

				}
			}
		}else if(m_selectionMode == SelectionMode::ISOSELECTION) {
			// iso selection
			int row1, col1;
			isometricToCartesian(m_mouseX + offsetX, m_mouseY + offsetY, row1, col1, cellWidth  * m_zoomFactor, cellHeight  * m_zoomFactor);

			int row2, col2;
			isometricToCartesian(mouseViewX + offsetX, mouseViewY + offsetY, row2, col2, cellWidth  * m_zoomFactor, cellHeight  * m_zoomFactor);

			int rowMin = std::min(row1, row2);
			int rowMax = std::max(row1, row2);
			int colMin = std::min(col1, col2);
			int colMax = std::max(col1, col2);


			for (int col = colMin; col <= colMax; col++) {
				for (int row = rowMin; row <= rowMax; row++) {
					for (int j = 0; j < m_layer.size(); j++) {
						if (isValid(row, col) && m_layer[j][col][row].first != -1) {
							if (!m_cells[m_layer[j][col][row].second].selected && m_cells[m_layer[j][col][row].second].visible) {
								m_cellCache.push_back(m_cells[m_layer[j][col][row].second]);
								m_cellCache.back().get().selected = true;
							}
							SingleSelectedCell defaultCell = { row, col, false };
							std::vector<SingleSelectedCell>::iterator it = std::find_if(m_singleCache.begin(), m_singleCache.end(), std::bind(FindSingleCell, std::placeholders::_1, defaultCell));
							if (it != m_singleCache.end())
								it->found = true;
						}
					}

				}
			}
		}else if(m_selectionMode == SelectionMode::MARKER) {
			//marker
			int row, col;
			isometricToCartesian(mouseViewX + offsetX, mouseViewY + offsetY, row, col, cellWidth  * m_zoomFactor, cellHeight  * m_zoomFactor);
			for (int j = 0; j < m_layer.size(); j++) {
				if (isValid(row, col) && m_layer[j][col][row].first != -1) {
					if (!m_cells[m_layer[j][col][row].second].selected && m_cells[m_layer[j][col][row].second].visible) {
						m_cellCache.push_back(m_cells[m_layer[j][col][row].second]);
						m_cellCache.back().get().selected = true;
					}
					SingleSelectedCell defaultCell = { row, col, false };
					std::vector<SingleSelectedCell>::iterator it = std::find_if(m_singleCache.begin(), m_singleCache.end(), std::bind(FindSingleCell, std::placeholders::_1, defaultCell));
					if (it != m_singleCache.end())
						it->found = true;
				}
			}
		}	
	}
}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);

		//clip
		//float mouseNDCX = (2.0f * event.x) / static_cast<float>(Application::Width) - 1.0f;
		//float mouseNDCY = 1.0f - (2.0f * event.y) / static_cast<float>(Application::Height);

		//view
		//float mouseViewX = 0.5f * (m_right * (mouseNDCX + 1.0f) + m_left   * (1.0f - mouseNDCX));
		//float mouseViewY = 0.5f * (m_top   * (mouseNDCY + 1.0f) + m_bottom * (1.0f - mouseNDCY));

		float mouseViewX = static_cast<float>(event.x);
		float mouseViewY = static_cast<float>(Application::Height - event.y);

		//world with zoom
		float mouseWorldX = mouseViewX + m_zoomFactor * (m_camera.getPositionX() + m_focusPointX) - m_focusPointX;
		float mouseWorldY = mouseViewY + m_zoomFactor * (m_camera.getPositionY() + m_focusPointY) - m_focusPointY;

		int row, col;
		isometricToCartesian(mouseWorldX, mouseWorldY, row, col, cellWidth  * m_zoomFactor, cellHeight  * m_zoomFactor);
		for (int j = 0; j < m_layer.size(); j++) {
			if (isValid(row, col) && m_layer[j][col][row].first != -1) {
				m_selectedCells.push_back(m_cells[m_layer[j][col][row].second]);
				Cell& cell = m_selectedCells.back();
				//cell.selected = !cell.selected;
				cell.selected = true;

				if (cell.selected && cell.visible) {
					m_singleCache.push_back({ row, col, false });
				}
				else {
					m_singleCache.erase(std::remove_if(m_singleCache.begin(), m_singleCache.end(), [](SingleSelectedCell cell) {return cell.found; }), m_singleCache.end());
				}
			}
		}
		m_mouseDown = true;
		m_mouseX = mouseViewX;
		m_mouseY = mouseViewY;
		m_curMouseX = m_mouseX;
		m_curMouseY = m_mouseY;

	} else if (event.button == 2u) {
		processCache(m_selectedCells, true, false, true);
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);

		float mouseViewX = static_cast<float>(event.x);
		float mouseViewY = static_cast<float>(Application::Height - event.y);

		m_singleCache.erase(std::remove_if(m_singleCache.begin(), m_singleCache.end(), [this](SingleSelectedCell cell) {
			if (cell.found || !m_mouseMove) {
				for (int j = 0; j < m_layer.size(); j++) {
					if (m_layer[j][cell.col][cell.row].first != -1) {
						m_cells[m_layer[j][cell.col][cell.row].second].visible = false;
						m_cells[m_layer[j][cell.col][cell.row].second].selected = true;
					}
				}
			}

			return cell.found || !m_mouseMove;
			})
			, m_singleCache.end()
			);
		processCache(m_cellCache, false, true, m_selectedCells, true);

		m_mouseDown = false;
		m_mouseMove = false;
		m_mouseX = mouseViewX;
		m_mouseY = mouseViewY;
		m_curMouseX = m_mouseX;
		m_curMouseY = m_mouseY;

	}
	else if (event.button == 2u) {
		//m_drawUi = true;
		Mouse::instance().detach();
	}
}

void Game::OnMouseWheel(Event::MouseWheelEvent& event) {
	if (event.direction == 1u) {
		m_zoomFactor = m_zoomFactor - 0.05f;
		m_zoomFactor = Math::Clamp(m_zoomFactor, 0.2f, 2.5f);
	}

	if (event.direction == 0u) {
		m_zoomFactor = m_zoomFactor + 0.05f;
		m_zoomFactor = Math::Clamp(m_zoomFactor, 0.2f, 2.5f);
	}
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
	m_mainRT.resize(Application::Width, Application::Height);

	m_left = m_camera.getLeftOrthographic();
	m_right = m_camera.getRightOrthographic();
	m_bottom = m_camera.getBottomOrthographic();
	m_top = m_camera.getTopOrthographic();

	m_focusPointX = static_cast<float>(Application::Width / 2);
	m_focusPointY = static_cast<float>(Application::Height / 2);
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
	ImGui::Checkbox("Use Culling", &m_useCulling);
	ImGui::Checkbox("Draw Culling Rect", &m_drawCullingRect);
	if (ImGui::Checkbox("Auto Redraw", &m_autoRedraw)) {
		m_redrawMap = !m_autoRedraw;
	}
	ImGui::SliderFloat("Screen Border", &m_screeBorder, 0.0f, 450.0f);
	ImGui::SliderFloat("Focus Point X", &m_focusPointX, -1600.0f, 1600.0f);
	ImGui::SliderFloat("Focus Point Y", &m_focusPointY, -1000.0f, 1000.0f);
	ImGui::SliderFloat("Enlarge Culling Rect", &m_enlargeBorder, 0.0f, 600.0f);

	int currentSelectionMode = m_selectionMode;
	if (ImGui::Combo("Mode", &currentSelectionMode, "Box Slection\0Iso Selection\0Marker\0Rasterizer\0\0")) {
		m_selectionMode = static_cast<SelectionMode>(currentSelectionMode);
	}

	if (m_selectionMode == SelectionMode::ISOSELECTION) {
		ImGui::Checkbox("Discrete Selection", &m_discreteSelection);
	}

	if (m_autoRedraw) {
		if (ImGui::Button("Redraw Map")) {
			m_redrawMap = true;
		}
	}

	
	int currentDierection = m_direction;
	if (ImGui::Combo("Direction", &currentDierection, "East\0North East Down\0North East Middle\0North East Up\0North\0North West Up\0North West Middle\0North West Dow\0West\0South West Up\0South West Middle\\0South West Down\0South\0South East Down\0South East Middle\0South East Up\0\0")) {
		m_direction = static_cast<Enums::Direction16 > (currentDierection);
	}

	ImGui::SliderInt("Animation Frame", &m_animationFrame, 0, 13);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::processCache(std::vector<std::reference_wrapper<Cell>>& cache, bool visible, bool selected, bool clear) {
	std::for_each(cache.begin(), cache.end(), [visible, selected](std::reference_wrapper<Cell> const& cell) {
		cell.get().selected = selected;
		cell.get().visible = visible;

	});

	if (clear)
		cache.clear();
}

void Game::processCache(std::vector<std::reference_wrapper<Cell>>& cache, bool visible, bool selected, std::vector<std::reference_wrapper<Cell>>& storage, bool clearAfterCopy) {
	std::for_each(cache.begin(), cache.end(), [visible, selected](std::reference_wrapper<Cell> const& cell) {
		cell.get().selected = selected;
		cell.get().visible = visible;

	});

	storage.insert(storage.end(), cache.begin(), cache.end());

	if (clearAfterCopy)
		cache.clear();
}

void Game::SkipFileKey(std::ifstream & read) {
	read.ignore(std::numeric_limits<std::streamsize>::max(), ':');
	while (read.peek() == ' ' || read.peek() == '\t')
		read.ignore();
}


void Game::loadMap(std::string name) {
	std::ifstream read(name);
	char buffer[MAX_ESTRING_LENGTH];
	memset(buffer, 0, sizeof(buffer));

	numColumns = 0;
	numRows = 0;
	cellWidth = 0;
	cellHeight = 0;
	numLayers = 0;
	int row = 0;
	int column = 0;

	while (read.peek() == '#')
		read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	for (int i = 0; i < 6; ++i) {
		SkipFileKey(read);
		switch (i) {
		case 0: read >> numColumns; break;
		case 1: read >> numRows;	break;
		case 2: read >> cellWidth;	break;
		case 3: read >> cellHeight; break;
		case 4: read >> numLayers;	break;
		case 5: read.getline(buffer, sizeof(buffer), '\n'); break;		// tileSet filename
		}

		read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');		// skip the rest of the line (BUGFIX: and the # begin layer def comment line)
	}

	m_layer.resize(numLayers);

	unsigned int layer = 0;
	read.ignore(std::numeric_limits<std::streamsize>::max(), '{');			// ignore up past "Layers {"
	read.ignore(1, '\n');													// ignore the '\n' past '{'

	while (read.peek() != '}') {
		row = 0;
		column = 0;
		size_t tallestRenderBlock = 0;
		read.ignore(std::numeric_limits<std::streamsize>::max(), '{');			// ignore up past "layer_# {"
		read.ignore(1, '\n');													// ignore the '\n' past '{'
		m_layer[layer] = new std::pair<int, unsigned int>*[numColumns];
		for (int i = 0; i < numColumns; ++i)
			m_layer[layer][i] = new std::pair<int, unsigned int>[numRows];
		// read one layer
		while (read.peek() != '}') {
			int tileType = -1;
			read >> m_layer[layer][column][row].first;

			m_layer[layer][column][row].first--;

			if (m_layer[layer][column][row].first != -1) {

				const TextureRect& rect = TileSetManager::Get().getTileSet("map").getTextureRects()[m_layer[layer][column][row].first];
				float cartX = static_cast<float>(row);
				float cartY = static_cast<float>(column);
				cartesianToIsometric(cartX, cartY, cellWidth, cellHeight);
				m_cells.push_back({ rect, cartX, -cartY , false, true });
				m_layer[layer][column][row].second = m_cells.size() - 1;
			}

			if (read.peek() == '\n') {
				read.ignore(1, '\n');
				row = 0;
				column++;
			}
			else if (read.peek() == ',') {
				read.ignore(1, ',');
				row++;
				if (row >= numRows) {
					row = 0;
					column++;
				}
			}


		}
		read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		++layer;

	}
	read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

//isoX =  (cartX * CELL_WIDTH - cartY * CELL_HEIGHT) 
//isoY = -(cartX * CELL_WIDTH + cartY * CELL_HEIGHT)  * 0.5f
void Game::cartesianToIsometric(float & x, float & y, float cellWidth, float cellHeight) {
	float cartX = x * cellWidth;
	float cartY = y * cellHeight;
	x = (cartX - cartY);
	y = (cartX + cartY) * 0.5f;
}

//iso -> cart
//isoX =  (cartX * CELL_WIDTH - cartY * CELL_HEIGHT) 
//isoY = -(cartX * CELL_WIDTH + cartY * CELL_HEIGHT)  * 0.5f
//cartX = (isoX  + cartY * CELL_HEIGHT) / CELL_WIDTH
//cartY = (-2.0 * isoY  - cartX * CELL_WIDTH) / CELL_HEIGHT
//
//cartX = (isoX - 2.0 * isoY - cartX * CELL_WIDTH) / CELL_WIDTH
//cartX =  (0.5f * isoX - isoY) /CELL_WIDTH 
//
//cartY = (-2.0 * isoY  - isoX  - cartY * CELL_HEIGHT) / CELL_HEIGHT
//cartY =-(0.5f * isoX + isoY) / CELL_HEIGHT
void Game::isometricToCartesian(float& x, float& y, float cellWidth, float cellHeight) {
	float isoX = x;
	float isoY = y;

	x = (0.5f * isoX - isoY) / cellWidth;
	y = -(0.5f * isoX + isoY) / cellHeight;
}

void Game::isometricToCartesian(float x, float y, int& row, int& col, float cellWidth, float cellHeight) {
	float isoX = x;
	float isoY = y;

	x = (0.5f * isoX - isoY) / cellWidth;
	y = -(0.5f * isoX + isoY) / cellHeight;

	row = static_cast<int>(std::roundf(x));
	col = static_cast<int>(std::roundf(y)) + 1;
}

void Game::isometricToCartesian(float x, float y, int& row, int& col, float cellWidth, float cellHeight, int min, int max) {
	float isoX = x;
	float isoY = y;

	x = (0.5f * isoX - isoY) / cellWidth;
	y = -(0.5f * isoX + isoY) / cellHeight;

	row = static_cast<int>(std::roundf(x));
	col = static_cast<int>(std::roundf(y)) + 1;

	row = Math::Clamp(row, min, max);
	col = Math::Clamp(col, min, max);
}

void Game::isometricToRow(float x, float y, int& row, float cellWidth) {
	float isoX = x;
	float isoY = y;

	x = (0.5f * isoX - isoY) / cellWidth;
	row = static_cast<int>(std::roundf(x));
}

void Game::isometricToRow(float x, float y, int& row, float cellWidth, int min, int max) {
	float isoX = x;
	float isoY = y;

	x = (0.5f * isoX - isoY) / cellWidth;
	row = static_cast<int>(std::roundf(x));

	row = Math::Clamp(row, min, max);
}

void Game::isometricToCol(float x, float y, int& col, float cellHeight) {
	float isoX = x;
	float isoY = y;

	y = -(0.5f * isoX + isoY) / cellHeight;
	col = static_cast<int>(std::roundf(y)) + 1;
}

void Game::isometricToCol(float x, float y, int& col, float cellHeight, int min, int max) {
	float isoX = x;
	float isoY = y;

	y = -(0.5f * isoX + isoY) / cellHeight;
	col = static_cast<int>(std::roundf(y)) + 1;

	col = Math::Clamp(col, min, max);
}

bool Game::isValid(const int row, const int column) const {
	return (row >= 0 && row < 128 && column >= 0 && column < 128);
}

void Game::culling() {
	if (!m_useCulling) return;
	Vector3f m_position = m_camera.getPosition();

	m_cullingVertices[0] = Vector2f(m_left + m_screeBorder, m_bottom + m_screeBorder) + Vector2f(m_zoomFactor * (m_position[0] + m_focusPointX) - m_focusPointX, m_zoomFactor * (m_position[1] + m_focusPointY) - m_focusPointY);
	m_cullingVertices[1] = Vector2f(m_left + m_screeBorder, m_top - m_screeBorder) + Vector2f(m_zoomFactor * (m_position[0] + m_focusPointX) - m_focusPointX, m_zoomFactor * (m_position[1] + m_focusPointY) - m_focusPointY);
	m_cullingVertices[2] = Vector2f(m_right - m_screeBorder, m_top - m_screeBorder) + Vector2f(m_zoomFactor * (m_position[0] + m_focusPointX) - m_focusPointX, m_zoomFactor * (m_position[1] + m_focusPointY) - m_focusPointY);
	m_cullingVertices[3] = Vector2f(m_right - m_screeBorder, m_bottom + m_screeBorder) + Vector2f(m_zoomFactor * (m_position[0] + m_focusPointX) - m_focusPointX, m_zoomFactor * (m_position[1] + m_focusPointY) - m_focusPointY);

	int rowMin, rowMax, colMin, colMax;
	isometricToCol(m_cullingVertices[0][0] - m_enlargeBorder, m_cullingVertices[0][1] - m_enlargeBorder, colMax, cellHeight * m_zoomFactor, 0, 128);
	isometricToRow(m_cullingVertices[3][0] + m_enlargeBorder, m_cullingVertices[3][1] - m_enlargeBorder, rowMax, cellWidth  * m_zoomFactor, 0, 128);
	isometricToRow(m_cullingVertices[1][0], m_cullingVertices[1][1], rowMin, cellWidth  * m_zoomFactor, 0, 128);
	isometricToCol(m_cullingVertices[2][0], m_cullingVertices[2][1], colMin, cellHeight * m_zoomFactor, 0, 128);

	m_visibleCells.clear();


	for (int j = 0; j < m_layer.size(); j++) {
		for (int y = colMin; y < colMax; y++) {
			for (int x = rowMin; x < rowMax; x++) {
				if (m_layer[j][y][x].first != -1) {
					m_visibleCells.push_back(m_cells[m_layer[j][y][x].second]);
				}

			}
		}
	}
}

void Game::drawCullingRect() {
	Vector3f m_position = m_camera.getPosition();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&m_camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(m_focusPointX - m_zoomFactor * (m_position[0] + m_focusPointX), m_focusPointY - m_zoomFactor * (m_position[1] + m_focusPointY), 0.0f);

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);

	glVertex3f(m_cullingVertices[0][0], m_cullingVertices[0][1], 0.0f);
	glVertex3f(m_cullingVertices[1][0], m_cullingVertices[1][1], 0.0f);
	glVertex3f(m_cullingVertices[2][0], m_cullingVertices[2][1], 0.0f);
	glVertex3f(m_cullingVertices[3][0], m_cullingVertices[3][1], 0.0f);

	glEnd();
	glPopMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void  Game::drawMouseRect() {
	if (!m_mouseDown || m_selectionMode == SelectionMode::MARKER || m_selectionMode == SelectionMode::RASTERIZER) return;

	float offsetX = m_zoomFactor * (m_camera.getPositionX() + m_focusPointX) - m_focusPointX;
	float offsetY = m_zoomFactor * (m_camera.getPositionY() + m_focusPointY) - m_focusPointY;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(2.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&m_camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	

	if (m_selectionMode == SelectionMode::BOXSELECTION) {
		float left = std::min(m_mouseX, m_curMouseX);
		float bottom = std::min(m_mouseY, m_curMouseY);
		float top = std::max(m_mouseY, m_curMouseY);
		float right = std::max(m_mouseX, m_curMouseX);

		glBegin(GL_QUADS);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(left, bottom, 0.0f);
		glVertex3f(left, top, 0.0f);
		glVertex3f(right, top, 0.0f);
		glVertex3f(right, bottom, 0.0f);
		glEnd();

	}else if(m_selectionMode == SelectionMode::ISOSELECTION) {


		glLoadMatrixf(&m_camera.getViewMatrix()[0][0]);
		int row1, col1, row2, col2;
		float p00, p01, p10, p11;
		if (m_discreteSelection) {
			isometricToCartesian(m_mouseX + offsetX, m_mouseY + offsetY, row1, col1, cellWidth  * m_zoomFactor, cellHeight  * m_zoomFactor);
			isometricToCartesian(m_curMouseX + offsetX, m_curMouseY + offsetY, row2, col2, cellWidth  * m_zoomFactor, cellHeight  * m_zoomFactor);
		}else {
			p00 = m_mouseX + offsetX;
			p01 = m_mouseY + offsetY;
			p10 = m_curMouseX + offsetX;
			p11 = m_curMouseY + offsetY;
			isometricToCartesian(p00, p01, cellWidth  * m_zoomFactor, cellHeight  * m_zoomFactor);
			isometricToCartesian(p10, p11, cellWidth  * m_zoomFactor, cellHeight  * m_zoomFactor);
		}

		float rowMin = m_discreteSelection ? static_cast<float>(std::min(row1, row2)) : std::min(p00, p10);
		float rowMax = m_discreteSelection ? static_cast<float>(std::max(row1, row2)) : std::max(p00, p10);
		float colMin = m_discreteSelection ? static_cast<float>(std::min(col1, col2) - 1) : std::min(p01, p11);
		float colMax = m_discreteSelection ? static_cast<float>(std::max(col1, col2)) : std::max(p01, p11);

		glBegin(GL_QUADS);
		glColor3f(0.0f, 1.0f, 0.0f);

		float cartX = m_discreteSelection ? rowMax + 0.5f : rowMax + 1.0f;
		float cartY = m_discreteSelection ? colMax - 0.5f : colMax + 1.0f;
		cartesianToIsometric(cartX, cartY, cellWidth, cellHeight);
		glVertex3f(cartX * m_zoomFactor + (m_camera.getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor), -cartY * m_zoomFactor + (m_camera.getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor), 0.0f);

		cartX = m_discreteSelection ? rowMin - 0.5f : rowMin - 1.0f;
		cartY = m_discreteSelection ? colMax - 0.5f : colMax + 1.0f;
		cartesianToIsometric(cartX, cartY, cellWidth, cellHeight);
		glVertex3f(cartX * m_zoomFactor + (m_camera.getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor), -cartY * m_zoomFactor + (m_camera.getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor), 0.0f);

		cartX = m_discreteSelection ? rowMin - 0.5f : rowMin - 1.0f;
		cartY = m_discreteSelection ? colMin - 0.5f : colMin - 1.0f;
		cartesianToIsometric(cartX, cartY, cellWidth, cellHeight);
		glVertex3f(cartX * m_zoomFactor + (m_camera.getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor), -cartY * m_zoomFactor + (m_camera.getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor), 0.0f);

		cartX = m_discreteSelection ? rowMax + 0.5f : rowMax + 1.0f;
		cartY = m_discreteSelection ? colMin - 0.5f : colMin - 1.0f;
		cartesianToIsometric(cartX, cartY, cellWidth, cellHeight);
		glVertex3f(cartX * m_zoomFactor + (m_camera.getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor), -cartY * m_zoomFactor + (m_camera.getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor), 0.0f);
		glEnd();
	}
	

	glLineWidth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

bool Game::FindSingleCell(SingleSelectedCell const& s1, SingleSelectedCell const& s2) {
	return s1.row == s2.row && s1.col == s2.col;
} 

void Game::loadAnimation(std::string name) {
	std::ifstream readAnimations(name);

	char resourceFilename[128];

	Enums::Direction16 direction = Enums::Direction16::E;
	while (!readAnimations.eof()) {
		readAnimations >> resourceFilename;

		std::ifstream readAnimationDef(resourceFilename);
		int numAnimationFrames = 0;
		int framesPerSecond = 0;
		int loopInt = 0;
		AnimationLoopState loopMode;

		readAnimationDef >> numAnimationFrames >> framesPerSecond >> loopInt;

		switch (loopInt) {
			case 1: loopMode = AnimationLoopState::ONCE; break;
			case 2: loopMode = AnimationLoopState::REPEAT; break;
			default: loopMode = AnimationLoopState::REPEAT; break;
		}
		
		readAnimationDef.ignore(std::numeric_limits<std::streamsize>::max(), '{');
		while (!readAnimationDef.eof() && readAnimationDef.peek() != '}') {
			int subframeIndex;
			float normalizedTime;
			readAnimationDef >> subframeIndex >> normalizedTime;
			readAnimationDef.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			const TextureRect& rect = TileSetManager::Get().getTileSet("sHero_anm").getTextureRects()[direction * numAnimationFrames + subframeIndex];

			m_animationFrames.push_back({ rect , normalizedTime,  direction});
		}
		readAnimationDef.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		direction = static_cast<Enums::Direction16>(static_cast<int>(direction) + 1);
	}
}