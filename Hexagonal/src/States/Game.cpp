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

Camera Game::_Camera;
float Game::ZoomFactor;
float Game::FocusPointY;
float Game::FocusPointX;

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	_Camera = Camera();
	_Camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	_Camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	_Camera.lookAt(Vector3f(-800.0f, -1500.0f, 0.0f), Vector3f(-800.0f, -1500.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));

	_Camera.setRotationSpeed(0.1f);
	_Camera.setMovingSpeed(400.0f);


	ZoomFactor = 1.0f;

	m_left = _Camera.getLeftOrthographic();
	m_right = _Camera.getRightOrthographic();
	m_bottom = _Camera.getBottomOrthographic();
	m_top = _Camera.getTopOrthographic();

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
	loadCollision("Graphics/Maps/Tileset_defs/evilMaster2.etls");
	m_atlas = TileSetManager::Get().getTileSet("map").getAtlas();
	//Spritesheet::Safe("map", m_atlas);

	loadMap("res/EvilTown2.emap");

	FocusPointX = static_cast<float>(Application::Width / 2);
	FocusPointY = static_cast<float>(Application::Height / 2);

	TileSetManager::Get().getTileSet("sHero_anm").loadTileSet("Graphics/Animations/sHero/Controller_defs/sHero.bimg", 1280u, 1280u, true);
	//Spritesheet::Safe("sHero", TileSetManager::Get().getTileSet("sHero_anm").getAtlas());

	TileSetManager::Get().getTileSet("sArcher_anm").loadTileSet("Graphics/Animations/sArcher/Controller_defs/sArcher.bimg", 1280u, 1024u, true);
	//Spritesheet::Safe("sArcher", TileSetManager::Get().getTileSet("sArcher_anm").getAtlas());

	AnimationManager::Get().loadAnimation16("Graphics/Animations/sHero/Controller_defs/sHero.banim", TileSetManager::Get().getTileSet("sHero_anm").getTextureRects());
	AnimationManager::Get().loadAnimation8("Graphics/Animations/sArcher/Controller_defs/sArcher.banim", TileSetManager::Get().getTileSet("sArcher_anm").getTextureRects());

	PrefabManager::Get().addPrefab("sHero", TileSetManager::Get().getTileSet("sHero_anm"));
	PrefabManager::Get().getPrefab("sHero").setAnimationController(new eAnimationController("Graphics/Animations/sHero/Controller_defs/sHero.ectrl"));
	PrefabManager::Get().getPrefab("sHero").setBounds(Vector4f(-8.0f, -8.0f, 8.0f, 8.0f));
	PrefabManager::Get().getPrefab("sHero").setOffset(Vector2f(10.0f, -18.0f));
	PrefabManager::Get().getPrefab("sHero").setBoundingBox({ 16.0f, 0.0f, 40.0f, 64.0f });

	PrefabManager::Get().addPrefab("sArcher", TileSetManager::Get().getTileSet("sArcher_anm"));
	PrefabManager::Get().getPrefab("sArcher").setAnimationController(new eAnimationController("Graphics/Animations/sArcher/Controller_defs/sArcher.ectrl"));
	PrefabManager::Get().getPrefab("sArcher").setBounds(Vector4f(-8.0f, -8.0f, 8.0f, 8.0f));
	PrefabManager::Get().getPrefab("sArcher").setOffset(Vector2f(10.0f, -38.0f));
	PrefabManager::Get().getPrefab("sArcher").setBoundingBox({ 32.0f, 0.0f, 32.0f, 100.0f});

	m_entities.push_back(new Entity(PrefabManager::Get().getPrefab("sHero"), _Camera, ZoomFactor, FocusPointX, FocusPointY));
	m_entities[0]->setPosition({ -200.0f, -1110.0f });

	m_entities.push_back(new Entity(PrefabManager::Get().getPrefab("sArcher"), _Camera, ZoomFactor, FocusPointX, FocusPointY));
	m_entities[1]->setPosition({0.0f, -1120.0f});

	m_entities.push_back(new Entity(PrefabManager::Get().getPrefab("sHero"), _Camera, ZoomFactor, FocusPointX, FocusPointY));
	m_entities[2]->setPosition({ 200.0f, -1120.0f });
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
		directrion += Vector3f(0.0f, 1.0f * (1.0f / ZoomFactor), 0.0f);		
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		directrion += Vector3f(0.0f, -1.0f * (1.0f / ZoomFactor), 0.0f);		
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		directrion += Vector3f(-1.0f * (1.0f / ZoomFactor), 0.0f, 0.0f);
		m_background.addOffset(-0.001f);
		m_background.setSpeed(-0.005f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3f(1.0f * (1.0f / ZoomFactor), 0.0f, 0.0f);
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

	if (mouse.buttonPressed(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			//_Camera.rotate(dx, dy);
		}

		if (move) {
			_Camera.move(directrion * m_dt);
		}
	}
	m_trackball.idle();
	m_transform.fromMatrix(m_trackball.getTransform());

	m_background.update(m_dt);

	for (auto entity = m_entities.begin(); entity != m_entities.end(); entity++) {
		(*entity)->processInput();
		(*entity)->update(m_dt);
		
	}
}

void Game::render() {
	
	if (m_redrawMap) {
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
		shader->loadMatrix("u_transform", _Camera.getOrthographicMatrix() * _Camera.getViewMatrix());

		Spritesheet::Bind(m_atlas);
		for (auto cell : m_useCulling ? m_visibleCells : m_cells) {
			if (cell.visible) {			
				Batchrenderer::Get().addQuadAA(Vector4f(cell.posX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), cell.posY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), cell.rect.width * ZoomFactor, cell.rect.height * ZoomFactor), Vector4f(cell.rect.textureOffsetX, cell.rect.textureOffsetY, cell.rect.textureWidth, cell.rect.textureHeight), cell.selected ? Vector4f(0.5f, 0.5f, 0.5f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f), cell.rect.frame);
			}
		}

		Batchrenderer::Get().drawBufferRaw();
		Spritesheet::Unbind();

		for (auto entity = m_entities.begin(); entity != m_entities.end(); entity++) {
			const TextureRect& rect = (*entity)->m_animationController->currentFrame->rect;
			Spritesheet::Bind((*entity)->prefab.tileSet.getAtlas());
			Batchrenderer::Get().addQuadAA(Vector4f((*entity)->m_position[0] * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), (*entity)->m_position[1] * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), rect.width * ZoomFactor, rect.height * ZoomFactor), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), (*entity)->m_isMarked ? Vector4f(0.8f, 0.2f, 0.2f, 1.0f) : (*entity)->m_isSelected ? Vector4f(0.5f, 0.5f, 0.5f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
			Batchrenderer::Get().drawBufferRaw();
			Spritesheet::Unbind();
		}

		shader->unuse();
		glDisable(GL_BLEND);

		for (auto cell : m_collisionCells) {
			drawIsometricRect((cell.posX + 32.0f) * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), (cell.posY + 32.0f) * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), cell.collisionRect * ZoomFactor, Vector4f(1.0f, 0.0f, 1.0f, 1.0f));
		}

		for (auto entity = m_entities.begin(); entity != m_entities.end(); entity++) {
			drawIsometricRect(((*entity)->m_position[0]) * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), ((*entity)->m_position[1]) * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), (*entity)->prefab.bounds * ZoomFactor, (*entity)->prefab.offset * ZoomFactor, Vector4f(1.0f, 0.0f, 1.0f, 1.0f));
			(*entity)->updateGridBounds();

			for (int y = (*entity)->m_minY; y <= (*entity)->m_maxY; y++) {
				for (int x = (*entity)->m_minX; x <= (*entity)->m_maxX; x++) {
					drawIsometricRect(x, y, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
				}
			}

			drawClickBox((*entity)->m_position[0] + (*entity)->prefab.boundingBox.posX, (*entity)->m_position[1] + (*entity)->prefab.boundingBox.posY, +(*entity)->prefab.boundingBox.width, (*entity)->prefab.boundingBox.height);

			(*entity)->m_movementPlaner->DebugDraw();
		}
		

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
	shader->unuse();

	if (m_drawUi)
		renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);

	if (m_selectionMode == SelectionMode::ENTITY) {
		if (m_mouseDown) {
			float offsetX = ZoomFactor * (_Camera.getPositionX() + FocusPointX) - FocusPointX;
			float offsetY = ZoomFactor * (_Camera.getPositionY() + FocusPointY) - FocusPointY;

			float mouseViewX = static_cast<float>(event.x);
			float mouseViewY = static_cast<float>(Application::Height - event.y);

			m_curMouseX = mouseViewX;
			m_curMouseY = mouseViewY;
			m_mouseMove = true;

			float left = std::min(m_mouseX, m_curMouseX);
			float bottom = std::min(m_mouseY, m_curMouseY);			
			float right = std::max(m_mouseX, m_curMouseX);
			float top = std::max(m_mouseY, m_curMouseY);

			for (auto entity = m_entities.begin(); entity != m_entities.end(); entity++) {
				(*entity)->mark(left, bottom, right, top);
			}
		}
	}else {

		if (m_mouseDown) {
			m_mouseMove = true;
			for (auto cell : m_singleCache) {
				cell.found = false;
			}

			if (m_selectionMode != SelectionMode::MARKER)
				processCache(m_cellCache, true, false, true);

			float offsetX = ZoomFactor * (_Camera.getPositionX() + FocusPointX) - FocusPointX;
			float offsetY = ZoomFactor * (_Camera.getPositionY() + FocusPointY) - FocusPointY;

			float mouseViewX = static_cast<float>(event.x);
			float mouseViewY = static_cast<float>(Application::Height - event.y);

			m_curMouseX = mouseViewX;
			m_curMouseY = mouseViewY;
			if (m_selectionMode == SelectionMode::RASTERIZER) {
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
				Math::isometricToCartesian(corners[0][0] + offsetX, corners[0][1] + offsetY, point01, point02, cellWidth * ZoomFactor, cellHeight * ZoomFactor);

				int point11, point12;
				Math::isometricToCartesian(corners[1][0] + offsetX, corners[1][1] + offsetY, point11, point12, cellWidth * ZoomFactor, cellHeight * ZoomFactor);

				int point21, point22;
				Math::isometricToCartesian(corners[2][0] + offsetX, corners[2][1] + offsetY, point21, point22, cellWidth * ZoomFactor, cellHeight * ZoomFactor);

				int point31, point32;
				Math::isometricToCartesian(corners[3][0] + offsetX, corners[3][1] + offsetY, point31, point32, cellWidth * ZoomFactor, cellHeight * ZoomFactor);


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


			}else if (m_selectionMode == SelectionMode::BOXSELECTION) {
				//box selection
				int left = static_cast<int>(std::min(m_mouseX, mouseViewX));
				int right = static_cast<int>(std::max(m_mouseX, mouseViewX));
				int bottom = static_cast<int>(std::min(m_mouseY, mouseViewY));
				int top = static_cast<int>(std::max(m_mouseY, mouseViewY));

				for (int x = left; x < right; x = x + cellWidth * ZoomFactor) {
					for (int y = bottom; y < top; y = y + cellHeight * 0.5f * ZoomFactor) {
						int row, col;
						Math::isometricToCartesian(x + offsetX, y + offsetY, row, col, cellWidth  * ZoomFactor, cellHeight  * ZoomFactor);
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
			}else if (m_selectionMode == SelectionMode::ISOSELECTION) {
				// iso selection
				int row1, col1;
				Math::isometricToCartesian(m_mouseX + offsetX, m_mouseY + offsetY, row1, col1, cellWidth  * ZoomFactor, cellHeight  * ZoomFactor);

				int row2, col2;
				Math::isometricToCartesian(mouseViewX + offsetX, mouseViewY + offsetY, row2, col2, cellWidth  * ZoomFactor, cellHeight  * ZoomFactor);

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
			}else if (m_selectionMode == SelectionMode::MARKER) {
				//marker
				int row, col;
				Math::isometricToCartesian(mouseViewX + offsetX, mouseViewY + offsetY, row, col, cellWidth  * ZoomFactor, cellHeight  * ZoomFactor);
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
}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {

	if (m_selectionMode == SelectionMode::ENTITY) {
		
		for (auto entity = m_entities.begin(); entity != m_entities.end(); entity++) {
				(*entity)->processInput(event.x, event.y, event.button, true);
		}

		if (event.button == 1u) {
			float mouseViewX = static_cast<float>(event.x);
			float mouseViewY = static_cast<float>(Application::Height - event.y);

			m_mouseDown = true;
			m_mouseX = mouseViewX;
			m_mouseY = mouseViewY;
			m_curMouseX = m_mouseX;
			m_curMouseY = m_mouseY;
		}
		
	}else {
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
			float mouseWorldX = mouseViewX + ZoomFactor * (_Camera.getPositionX() + FocusPointX) - FocusPointX;
			float mouseWorldY = mouseViewY + ZoomFactor * (_Camera.getPositionY() + FocusPointY) - FocusPointY;

			int row, col;
			Math::isometricToCartesian(mouseWorldX, mouseWorldY, row, col, cellWidth  * ZoomFactor, cellHeight  * ZoomFactor);

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
			
		}
	}

	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	
	if (m_selectionMode == SelectionMode::ENTITY) {
		for (auto entity = m_entities.begin(); entity != m_entities.end(); entity++) {
			if(m_mouseMove && event.button == Event::MouseButtonEvent::MouseButton::BUTTON_LEFT)
				(*entity)->select();
			else
				(*entity)->processInput(event.x, event.y, event.button, false);		
		}	

		m_mouseDown = false;
		m_mouseMove = false;
		m_mouseX = static_cast<float>(event.x);
		m_mouseY = static_cast<float>(Application::Height - event.y);
		m_curMouseX = m_mouseX;
		m_curMouseY = m_mouseY;
	}else {
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
	}

	if (event.button == 2u) {
		//m_drawUi = true;
		Mouse::instance().detach();
	}
}

void Game::OnMouseWheel(Event::MouseWheelEvent& event) {
	if (event.direction == 1u) {
		ZoomFactor = ZoomFactor - 0.05f;
		ZoomFactor = Math::Clamp(ZoomFactor, 0.2f, 2.5f);
	}

	if (event.direction == 0u) {
		ZoomFactor = ZoomFactor + 0.05f;
		ZoomFactor = Math::Clamp(ZoomFactor, 0.2f, 2.5f);
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
	_Camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	_Camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_mainRT.resize(Application::Width, Application::Height);

	m_left = _Camera.getLeftOrthographic();
	m_right = _Camera.getRightOrthographic();
	m_bottom = _Camera.getBottomOrthographic();
	m_top = _Camera.getTopOrthographic();

	FocusPointX = static_cast<float>(Application::Width / 2);
	FocusPointY = static_cast<float>(Application::Height / 2);
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
	ImGui::SliderFloat("Focus Point X", &FocusPointX, -1600.0f, 1600.0f);
	ImGui::SliderFloat("Focus Point Y", &FocusPointY, -1000.0f, 1000.0f);
	ImGui::SliderFloat("Enlarge Culling Rect", &m_enlargeBorder, 0.0f, 600.0f);

	int currentSelectionMode = m_selectionMode;
	if (ImGui::Combo("Selection Mode", &currentSelectionMode, "Box Selection\0Iso Selection\0Marker\0Rasterizer\0Entity\0\0")) {
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
				const Vector4f& collRect = colAndBlockId[m_layer[layer][column][row].first][0] == 0 ? defaultAABBList[0] : defaultAABBList[colAndBlockId[m_layer[layer][column][row].first][0] - 1];
				const std::array<unsigned int, 3>& blockRect = colAndBlockId[m_layer[layer][column][row].first][1] == 0 ? defaultRenderBlockSizes[0] : defaultRenderBlockSizes[colAndBlockId[m_layer[layer][column][row].first][1] - 1];

				float cartX = static_cast<float>(row);
				float cartY = static_cast<float>(column);
				Math::cartesianToIsometric(cartX, cartY, cellWidth, cellHeight);
				m_cells.push_back({ rect, cartX, -cartY, false, true,  collRect , blockRect, colAndBlockId[m_layer[layer][column][row].first][0] != 0 });
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
	read.close();
}

bool Game::isValid(const int row, const int column) const {
	return (row >= 0 && row < 128 && column >= 0 && column < 128);
}

void Game::culling() {
	if (!m_useCulling) return;
	Vector3f m_position = _Camera.getPosition();

	m_cullingVertices[0] = Vector2f(m_left + m_screeBorder, m_bottom + m_screeBorder) + Vector2f(ZoomFactor * (m_position[0] + FocusPointX) - FocusPointX, ZoomFactor * (m_position[1] + FocusPointY) - FocusPointY);
	m_cullingVertices[1] = Vector2f(m_left + m_screeBorder, m_top - m_screeBorder) + Vector2f(ZoomFactor * (m_position[0] + FocusPointX) - FocusPointX, ZoomFactor * (m_position[1] + FocusPointY) - FocusPointY);
	m_cullingVertices[2] = Vector2f(m_right - m_screeBorder, m_top - m_screeBorder) + Vector2f(ZoomFactor * (m_position[0] + FocusPointX) - FocusPointX, ZoomFactor * (m_position[1] + FocusPointY) - FocusPointY);
	m_cullingVertices[3] = Vector2f(m_right - m_screeBorder, m_bottom + m_screeBorder) + Vector2f(ZoomFactor * (m_position[0] + FocusPointX) - FocusPointX, ZoomFactor * (m_position[1] + FocusPointY) - FocusPointY);

	int rowMin, rowMax, colMin, colMax;
	Math::isometricToCol(m_cullingVertices[0][0] - m_enlargeBorder, m_cullingVertices[0][1] - m_enlargeBorder, colMax, cellHeight * ZoomFactor, 0, 128);
	Math::isometricToRow(m_cullingVertices[3][0] + m_enlargeBorder, m_cullingVertices[3][1] - m_enlargeBorder, rowMax, cellWidth  * ZoomFactor, 0, 128);
	Math::isometricToRow(m_cullingVertices[1][0], m_cullingVertices[1][1], rowMin, cellWidth  * ZoomFactor, 0, 128);
	Math::isometricToCol(m_cullingVertices[2][0], m_cullingVertices[2][1], colMin, cellHeight * ZoomFactor, 0, 128);

	m_visibleCells.clear();
	m_collisionCells.clear();

	for (int j = 0; j < m_layer.size(); j++) {
		for (int y = colMin; y < colMax; y++) {
			for (int x = rowMin; x < rowMax; x++) {
				if (m_layer[j][y][x].first != -1) {

					m_visibleCells.push_back(m_cells[m_layer[j][y][x].second]);
					
					if (m_cells[m_layer[j][y][x].second].hasCollision) {
						m_collisionCells.push_back(m_cells[m_layer[j][y][x].second]);
					}
				}

			}
		}
	}
}

void Game::drawCullingRect() {
	Vector3f m_position = _Camera.getPosition();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&_Camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(FocusPointX - ZoomFactor * (m_position[0] + FocusPointX), FocusPointY - ZoomFactor * (m_position[1] + FocusPointY), 0.0f);

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);

	glVertex3f(m_cullingVertices[0][0], m_cullingVertices[0][1], 0.0f);
	glVertex3f(m_cullingVertices[1][0], m_cullingVertices[1][1], 0.0f);
	glVertex3f(m_cullingVertices[2][0], m_cullingVertices[2][1], 0.0f);
	glVertex3f(m_cullingVertices[3][0], m_cullingVertices[3][1], 0.0f);

	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void  Game::drawMouseRect() {
	if (!m_mouseDown || m_selectionMode == SelectionMode::MARKER || m_selectionMode == SelectionMode::RASTERIZER) return;

	float offsetX = ZoomFactor * (_Camera.getPositionX() + FocusPointX) - FocusPointX;
	float offsetY = ZoomFactor * (_Camera.getPositionY() + FocusPointY) - FocusPointY;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(2.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&_Camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	

	if (m_selectionMode == SelectionMode::BOXSELECTION || m_selectionMode == SelectionMode::ENTITY) {
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


		glLoadMatrixf(&_Camera.getViewMatrix()[0][0]);
		int row1, col1, row2, col2;
		float p00, p01, p10, p11;
		if (m_discreteSelection) {
			Math::isometricToCartesian(m_mouseX + offsetX, m_mouseY + offsetY, row1, col1, cellWidth  * ZoomFactor, cellHeight  * ZoomFactor);
			Math::isometricToCartesian(m_curMouseX + offsetX, m_curMouseY + offsetY, row2, col2, cellWidth  * ZoomFactor, cellHeight  * ZoomFactor);
		}else {
			p00 = m_mouseX + offsetX;
			p01 = m_mouseY + offsetY;
			p10 = m_curMouseX + offsetX;
			p11 = m_curMouseY + offsetY;
			Math::isometricToCartesian(p00, p01, cellWidth  * ZoomFactor, cellHeight  * ZoomFactor);
			Math::isometricToCartesian(p10, p11, cellWidth  * ZoomFactor, cellHeight  * ZoomFactor);
		}

		float rowMin = m_discreteSelection ? static_cast<float>(std::min(row1, row2)) : std::min(p00, p10);
		float rowMax = m_discreteSelection ? static_cast<float>(std::max(row1, row2)) : std::max(p00, p10);
		float colMin = m_discreteSelection ? static_cast<float>(std::min(col1, col2) - 1) : std::min(p01, p11);
		float colMax = m_discreteSelection ? static_cast<float>(std::max(col1, col2)) : std::max(p01, p11);

		glBegin(GL_QUADS);
		glColor3f(0.0f, 1.0f, 0.0f);

		float cartX = m_discreteSelection ? rowMax + 0.5f : rowMax + 1.0f;
		float cartY = m_discreteSelection ? colMax - 0.5f : colMax + 1.0f;
		Math::cartesianToIsometric(cartX, cartY, cellWidth, cellHeight);
		glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);

		cartX = m_discreteSelection ? rowMin - 0.5f : rowMin - 1.0f;
		cartY = m_discreteSelection ? colMax - 0.5f : colMax + 1.0f;
		Math::cartesianToIsometric(cartX, cartY, cellWidth, cellHeight);
		glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);

		cartX = m_discreteSelection ? rowMin - 0.5f : rowMin - 1.0f;
		cartY = m_discreteSelection ? colMin - 0.5f : colMin - 1.0f;
		Math::cartesianToIsometric(cartX, cartY, cellWidth, cellHeight);
		glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);

		cartX = m_discreteSelection ? rowMax + 0.5f : rowMax + 1.0f;
		cartY = m_discreteSelection ? colMin - 0.5f : colMin - 1.0f;
		Math::cartesianToIsometric(cartX, cartY, cellWidth, cellHeight);
		glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);
		glEnd();
	}
	

	glLineWidth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

bool Game::FindSingleCell(SingleSelectedCell const& s1, SingleSelectedCell const& s2) {
	return s1.row == s2.row && s1.col == s2.col;
} 

void Game::loadCollision(std::string name) {
	std::ifstream read(name);

	read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	char buffer[MAX_ESTRING_LENGTH];
	memset(buffer, 0, sizeof(buffer));
	read.getline(buffer, sizeof(buffer), '\n');

	enum {
		LOADING_DEFAULT_COLLISION,
		LOADING_DEFAULT_RENDERBLOCKS,
		LOADING_TILESET
	};
	int readState = LOADING_DEFAULT_COLLISION;

	read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');			// skip the third line comment
	

	while (readState == LOADING_DEFAULT_COLLISION) {
		memset(buffer, 0, sizeof(buffer));
		read.getline(buffer, sizeof(buffer), ':');							// collision shape text


		std::string collisionShape = buffer;
		if (collisionShape.compare("eBounds") == 0) {
			unsigned int width = 0;
			unsigned int height = 0;
			unsigned int xOffset = 0;
			unsigned int yOffset = 0;
			read >> width >> height >> xOffset >> yOffset;

			defaultAABBList.push_back({ static_cast<float>(width) , static_cast<float>(height) , static_cast<float>(xOffset) , static_cast<float>(yOffset)});
		}

		read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');		// skip the rest of the line
		if (read.peek() == '#') {
			read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');	// skip the renderBlockSize rules comment
			readState = LOADING_DEFAULT_RENDERBLOCKS;
		}
	}

	
	while (readState == LOADING_DEFAULT_RENDERBLOCKS) {
		SkipFileKey(read);													// rbSize text
		unsigned int width = 0;
		unsigned int height = 0;
		unsigned int depth = 0;
		read >> width >> height >> depth;
		
		defaultRenderBlockSizes.push_back({ width, height, depth });
		read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');		// skip the rest of the line
		if (read.peek() == '#') {
			read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');	// begin tile type definitions comment
			SkipFileKey(read);												// num_tiles text
			readState = LOADING_TILESET;
		}
	}

	

	size_t numTiles = 0;
	read >> numTiles;
	read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	while (!read.eof()) {	// readState == LOADING_TILESET
							// read a source image name
		memset(buffer, 0, sizeof(buffer));
		read.getline(buffer, sizeof(buffer), '\n');
		
		// get all subframe indexes for the eImage (separated by spaces), everything after '#' is ignored
		while (read.peek() != '#') {
			int subframeIndex;
			int colliderType;
			int renderBlockType;

			read >> subframeIndex >> colliderType >> renderBlockType;
			colAndBlockId.push_back({ colliderType, renderBlockType});
			read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	read.close();
}

void Game::drawIsometricRect(float posX, float posY, Vector4f sizeOffset, Vector4f color) {
	std::array<Vector2f, 4> fPoints;
	fPoints[0] = Vector2f(sizeOffset[2], sizeOffset[3]);
	fPoints[1] = Vector2f(sizeOffset[2], sizeOffset[3] + sizeOffset[1]);
	fPoints[2] = Vector2f(sizeOffset[2] + sizeOffset[0], sizeOffset[3] + sizeOffset[1]);
	fPoints[3] = Vector2f(sizeOffset[2] + sizeOffset[0], sizeOffset[3]);

	Math::cartesianToIsometric(fPoints[0][0], fPoints[0][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[1][0], fPoints[1][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[2][0], fPoints[2][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[3][0], fPoints[3][1], 1.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&_Camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&_Camera.getViewMatrix()[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor4f(color[0], color[1], color[2], color[3]);

	glVertex3f(posX + fPoints[0][0], posY - fPoints[0][1], 0.0f);
	glVertex3f(posX + fPoints[1][0], posY - fPoints[1][1], 0.0f);
	glVertex3f(posX + fPoints[2][0], posY - fPoints[2][1], 0.0f);
	glVertex3f(posX + fPoints[3][0], posY - fPoints[3][1], 0.0f);

	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Game::drawIsometricRect(float posX, float posY, Vector4f bounds, Vector2f offset, Vector4f color) {
	std::array<Vector2f, 4> fPoints;

	fPoints[0] = Vector2f(bounds[0] + offset[0], bounds[1] + offset[1]);
	fPoints[1] = Vector2f(bounds[0] + offset[0], bounds[3] + offset[1]);
	fPoints[2] = Vector2f(bounds[2] + offset[0], bounds[3] + offset[1]);
	fPoints[3] = Vector2f(bounds[2] + offset[0], bounds[1] + offset[1]);

	Math::cartesianToIsometric(fPoints[0][0], fPoints[0][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[1][0], fPoints[1][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[2][0], fPoints[2][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[3][0], fPoints[3][1], 1.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&_Camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&_Camera.getViewMatrix()[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor4f(color[0], color[1], color[2], color[3]);

	glVertex3f(posX + fPoints[0][0], posY - fPoints[0][1], 0.0f);
	glVertex3f(posX + fPoints[1][0], posY - fPoints[1][1], 0.0f);
	glVertex3f(posX + fPoints[2][0], posY - fPoints[2][1], 0.0f);
	glVertex3f(posX + fPoints[3][0], posY - fPoints[3][1], 0.0f);

	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Game::drawIsometricRect(int posX, int posY, Vector4f color) {

	//if (!isValid(posX, posY)) return;

	float rowMin = posX - 0.5f;
	float rowMax = posX + 0.5f;
	float colMin = posY - 1.5f;
	float colMax = posY - 0.5f;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor4f(color[0], color[1], color[2], color[3]);

	float cartX = rowMax;
	float cartY = colMax;
	Math::cartesianToIsometric(cartX, cartY, cellWidth, cellHeight);
	glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);

	cartX = rowMin;
	cartY = colMax;
	Math::cartesianToIsometric(cartX, cartY, cellWidth, cellHeight);
	glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);

	cartX = rowMin;
	cartY = colMin;
	Math::cartesianToIsometric(cartX, cartY, cellWidth, cellHeight);
	glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);

	cartX = rowMax;
	cartY = colMin;
	Math::cartesianToIsometric(cartX, cartY, cellWidth, cellHeight);
	glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Game::drawClickBox(float posX, float posY, float width, float height) {

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&_Camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&_Camera.getViewMatrix()[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);

	posX = posX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor);
	posY = posY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor);

	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(posX, posY, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(posX, (posY + height * ZoomFactor), 0.0f);
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(posX + width * ZoomFactor, (posY + height * ZoomFactor), 0.0f);
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(posX + width * ZoomFactor, posY, 0.0f);
	glEnd();

}

void Game::DrawIsometricRect(float posX, float posY, Vector4f sizeOffset, Vector4f color) {
	std::array<Vector2f, 4> fPoints;
	fPoints[0] = Vector2f(sizeOffset[2], sizeOffset[3]);
	fPoints[1] = Vector2f(sizeOffset[2], sizeOffset[3] + sizeOffset[1]);
	fPoints[2] = Vector2f(sizeOffset[2] + sizeOffset[0], sizeOffset[3] + sizeOffset[1]);
	fPoints[3] = Vector2f(sizeOffset[2] + sizeOffset[0], sizeOffset[3]);

	Math::cartesianToIsometric(fPoints[0][0], fPoints[0][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[1][0], fPoints[1][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[2][0], fPoints[2][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[3][0], fPoints[3][1], 1.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&_Camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&_Camera.getViewMatrix()[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor4f(color[0], color[1], color[2], color[3]);

	posX = posX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor);
	posY = posY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor);

	glVertex3f(posX + fPoints[0][0] * ZoomFactor, posY - fPoints[0][1] * ZoomFactor, 0.0f);
	//glVertex3f(posX + fPoints[0][0] * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), posY - fPoints[0][1] * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);

	glVertex3f(posX + fPoints[1][0] * ZoomFactor, posY - fPoints[1][1] * ZoomFactor, 0.0f);
	//glVertex3f(posX + fPoints[1][0] * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), posY - fPoints[1][1] * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);

	glVertex3f(posX + fPoints[2][0] * ZoomFactor, posY - fPoints[2][1] * ZoomFactor, 0.0f);
	//glVertex3f(posX + fPoints[2][0] * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), posY - fPoints[2][1] * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);

	glVertex3f(posX + fPoints[3][0] * ZoomFactor, posY - fPoints[3][1] * ZoomFactor, 0.0f);
	//glVertex3f(posX + fPoints[3][0] * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), posY - fPoints[3][1] * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);


	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Game::DrawIsometricRect(float posX, float posY, Vector4f bounds, Vector2f offset, Vector4f color) {
	std::array<Vector2f, 4> fPoints;

	fPoints[0] = Vector2f(bounds[0] + offset[0], bounds[1] + offset[1]);
	fPoints[1] = Vector2f(bounds[0] + offset[0], bounds[3] + offset[1]);
	fPoints[2] = Vector2f(bounds[2] + offset[0], bounds[3] + offset[1]);
	fPoints[3] = Vector2f(bounds[2] + offset[0], bounds[1] + offset[1]);

	Math::cartesianToIsometric(fPoints[0][0], fPoints[0][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[1][0], fPoints[1][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[2][0], fPoints[2][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[3][0], fPoints[3][1], 1.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&_Camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&_Camera.getViewMatrix()[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor4f(color[0], color[1], color[2], color[3]);

	glVertex3f(posX + fPoints[0][0], posY - fPoints[0][1], 0.0f);
	glVertex3f(posX + fPoints[1][0], posY - fPoints[1][1], 0.0f);
	glVertex3f(posX + fPoints[2][0], posY - fPoints[2][1], 0.0f);
	glVertex3f(posX + fPoints[3][0], posY - fPoints[3][1], 0.0f);

	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Game::DrawIsometricRect(int posX, int posY, Vector4f color) {

	//if (!isValid(posX, posY)) return;

	float rowMin = posX - 0.5f;
	float rowMax = posX + 0.5f;
	float colMin = posY - 1.5f;
	float colMax = posY - 0.5f;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor4f(color[0], color[1], color[2], color[3]);

	float cartX = rowMax;
	float cartY = colMax;
	Math::cartesianToIsometric(cartX, cartY, CELL_WIDTH, CELL_HEIGHT);
	glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);

	cartX = rowMin;
	cartY = colMax;
	Math::cartesianToIsometric(cartX, cartY, CELL_WIDTH, CELL_HEIGHT);
	glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);

	cartX = rowMin;
	cartY = colMin;
	Math::cartesianToIsometric(cartX, cartY, CELL_WIDTH, CELL_HEIGHT);
	glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);

	cartX = rowMax;
	cartY = colMin;
	Math::cartesianToIsometric(cartX, cartY, CELL_WIDTH, CELL_HEIGHT);
	glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}