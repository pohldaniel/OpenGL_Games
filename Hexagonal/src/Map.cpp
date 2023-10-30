#include <engine/Batchrenderer.h>
#include <engine/Spritesheet.h>

#include "Map.h"
#include "Math.hpp"
#include "Application.h"
#include "Utils/Rasterizer.h"

Map::Map(const Camera& _camera) : camera(_camera) {
	//EventDispatcher::AddKeyboardListener(this);
	//EventDispatcher::AddMouseListener(this);

	m_focusPointX = static_cast<float>(Application::Width / 2);
	m_focusPointY = static_cast<float>(Application::Height / 2);

	m_left = camera.getLeftOrthographic();
	m_right = camera.getRightOrthographic();
	m_bottom = camera.getBottomOrthographic();
	m_top = camera.getTopOrthographic();
}

Map::~Map() {
	//EventDispatcher::RemoveKeyboardListener(this);
	//EventDispatcher::RemoveMouseListener(this);
}

void Map::drawRaw() {
	culling();
	for (auto cell : m_useCulling ? m_visibleCells : m_cells) {
		if (cell.visible) {

			Batchrenderer::Get().addQuadAA(Vector4f(cell.posX * m_zoomFactor + (camera.getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor), cell.posY * m_zoomFactor + (camera.getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor), cell.rect.width * m_zoomFactor, cell.rect.height * m_zoomFactor), Vector4f(cell.rect.textureOffsetX, cell.rect.textureOffsetY, cell.rect.textureWidth, cell.rect.textureHeight), cell.selected ? Vector4f(0.5f, 0.5f, 0.5f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f), cell.rect.frame);
		}
	}

	Batchrenderer::Get().drawBufferRaw();
}

void Map::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (m_selectionMode == Enums::SelectionMode::NONE) return;
	if (event.button == 1u) {
		//clip
		//float mouseNDCX = (2.0f * event.x) / static_cast<float>(Application::Width) - 1.0f;
		//float mouseNDCY = 1.0f - (2.0f * event.y) / static_cast<float>(Application::Height);

		//view
		//float mouseViewX = 0.5f * (m_right * (mouseNDCX + 1.0f) + m_left   * (1.0f - mouseNDCX));
		//float mouseViewY = 0.5f * (m_top   * (mouseNDCY + 1.0f) + m_bottom * (1.0f - mouseNDCY));

		float mouseViewX = static_cast<float>(event.x);
		float mouseViewY = static_cast<float>(Application::Height - event.y);

		//world with zoom
		float mouseWorldX = mouseViewX + m_zoomFactor * (camera.getPositionX() + m_focusPointX) - m_focusPointX;
		float mouseWorldY = mouseViewY + m_zoomFactor * (camera.getPositionY() + m_focusPointY) - m_focusPointY;

		int row, col;
		Math::isometricToCartesian(mouseWorldX, mouseWorldY, row, col, m_cellWidth  * m_zoomFactor, m_cellHeight  * m_zoomFactor);

		for (int j = 0; j < m_layer.size(); j++) {
			if (isValid(row, col) && m_layer[j][col][row].first != -1) {
				m_selectedCells.push_back(m_cells[m_layer[j][col][row].second]);
				Cell& cell = m_selectedCells.back();
				//cell.selected = !cell.selected;
				cell.selected = true;

				if (cell.selected && cell.visible) {
					m_singleCache.push_back({ row, col, false });
				}else {
					m_singleCache.erase(std::remove_if(m_singleCache.begin(), m_singleCache.end(), [](SingleSelectedCell cell) {return cell.found; }), m_singleCache.end());
				}
			}
		}
		m_mouseDown = true;
		m_mouseX = mouseViewX;
		m_mouseY = mouseViewY;
		m_curMouseX = m_mouseX;
		m_curMouseY = m_mouseY;

	}else if (event.button == 2u) {
		processCache(m_selectedCells, true, false, true);
	}
}

void Map::OnMouseMotion(Event::MouseMoveEvent& event) {
	if (m_selectionMode == Enums::SelectionMode::NONE) return;
	if (m_mouseDown) {
		m_mouseMove = true;
		for (auto cell : m_singleCache) {
			cell.found = false;
		}

		if (m_selectionMode != Enums::SelectionMode::MARKER)
			processCache(m_cellCache, true, false, true);

		float offsetX = m_zoomFactor * (camera.getPositionX() + m_focusPointX) - m_focusPointX;
		float offsetY = m_zoomFactor * (camera.getPositionY() + m_focusPointY) - m_focusPointY;

		float mouseViewX = static_cast<float>(event.x);
		float mouseViewY = static_cast<float>(Application::Height - event.y);

		m_curMouseX = mouseViewX;
		m_curMouseY = mouseViewY;
		if (m_selectionMode == Enums::SelectionMode::RASTERIZER) {
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
			Math::isometricToCartesian(corners[0][0] + offsetX, corners[0][1] + offsetY, point01, point02, m_cellWidth * m_zoomFactor, m_cellHeight * m_zoomFactor);

			int point11, point12;
			Math::isometricToCartesian(corners[1][0] + offsetX, corners[1][1] + offsetY, point11, point12, m_cellWidth * m_zoomFactor, m_cellHeight * m_zoomFactor);

			int point21, point22;
			Math::isometricToCartesian(corners[2][0] + offsetX, corners[2][1] + offsetY, point21, point22, m_cellWidth * m_zoomFactor, m_cellHeight * m_zoomFactor);

			int point31, point32;
			Math::isometricToCartesian(corners[3][0] + offsetX, corners[3][1] + offsetY, point31, point32, m_cellWidth * m_zoomFactor, m_cellHeight * m_zoomFactor);


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

		}else if (m_selectionMode == Enums::SelectionMode::BOXSELECTION) {
			//box selection
			int left = static_cast<int>(std::min(m_mouseX, mouseViewX));
			int right = static_cast<int>(std::max(m_mouseX, mouseViewX));
			int bottom = static_cast<int>(std::min(m_mouseY, mouseViewY));
			int top = static_cast<int>(std::max(m_mouseY, mouseViewY));

			for (int x = left; x < right; x = x + m_cellWidth * m_zoomFactor) {
				for (int y = bottom; y < top; y = y + m_cellHeight * 0.5f * m_zoomFactor) {
					int row, col;
					Math::isometricToCartesian(x + offsetX, y + offsetY, row, col, m_cellWidth  * m_zoomFactor, m_cellHeight  * m_zoomFactor);
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

		}else if (m_selectionMode == Enums::SelectionMode::ISOSELECTION) {
			// iso selection
			int row1, col1;
			Math::isometricToCartesian(m_mouseX + offsetX, m_mouseY + offsetY, row1, col1, m_cellWidth  * m_zoomFactor, m_cellHeight * m_zoomFactor);

			int row2, col2;
			Math::isometricToCartesian(mouseViewX + offsetX, mouseViewY + offsetY, row2, col2, m_cellWidth  * m_zoomFactor, m_cellHeight * m_zoomFactor);

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
		}else if (m_selectionMode == Enums::SelectionMode::MARKER) {
			//marker
			int row, col;
			Math::isometricToCartesian(mouseViewX + offsetX, mouseViewY + offsetY, row, col, m_cellWidth * m_zoomFactor, m_cellHeight * m_zoomFactor);
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

void Map::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (m_selectionMode == Enums::SelectionMode::NONE) return;
	if (event.button == 1u) {
		
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

void Map::OnMouseWheel(Event::MouseWheelEvent& event) {
	if (event.direction == 1u) {
		m_zoomFactor = m_zoomFactor - 0.05f;
		m_zoomFactor = Math::Clamp(m_zoomFactor, 0.2f, 2.5f);
	}

	if (event.direction == 0u) {
		m_zoomFactor = m_zoomFactor + 0.05f;
		m_zoomFactor = Math::Clamp(m_zoomFactor, 0.2f, 2.5f);
	}
}

void Map::resize(int deltaW, int deltaH) {
	m_left = camera.getLeftOrthographic();
	m_right = camera.getRightOrthographic();
	m_bottom = camera.getBottomOrthographic();
	m_top = camera.getTopOrthographic();

	m_focusPointX = static_cast<float>(Application::Width / 2);
	m_focusPointY = static_cast<float>(Application::Height / 2);
}

void Map::processCache(std::vector<std::reference_wrapper<Cell>>& cache, bool visible, bool selected, bool clear) {
	std::for_each(cache.begin(), cache.end(), [visible, selected](std::reference_wrapper<Cell> const& cell) {
		cell.get().selected = selected;
		cell.get().visible = visible;

	});

	if (clear)
		cache.clear();
}

void Map::processCache(std::vector<std::reference_wrapper<Cell>>& cache, bool visible, bool selected, std::vector<std::reference_wrapper<Cell>>& storage, bool clearAfterCopy) {
	std::for_each(cache.begin(), cache.end(), [visible, selected](std::reference_wrapper<Cell> const& cell) {
		cell.get().selected = selected;
		cell.get().visible = visible;

	});

	storage.insert(storage.end(), cache.begin(), cache.end());

	if (clearAfterCopy)
		cache.clear();
}

void Map::culling() {
	if (!m_useCulling) return;
	Vector3f m_position = camera.getPosition();

	m_cullingVertices[0] = Vector2f(m_left + m_screeBorder, m_bottom + m_screeBorder) + Vector2f(m_zoomFactor * (m_position[0] + m_focusPointX) - m_focusPointX, m_zoomFactor * (m_position[1] + m_focusPointY) - m_focusPointY);
	m_cullingVertices[1] = Vector2f(m_left + m_screeBorder, m_top - m_screeBorder) + Vector2f(m_zoomFactor * (m_position[0] + m_focusPointX) - m_focusPointX, m_zoomFactor * (m_position[1] + m_focusPointY) - m_focusPointY);
	m_cullingVertices[2] = Vector2f(m_right - m_screeBorder, m_top - m_screeBorder) + Vector2f(m_zoomFactor * (m_position[0] + m_focusPointX) - m_focusPointX, m_zoomFactor * (m_position[1] + m_focusPointY) - m_focusPointY);
	m_cullingVertices[3] = Vector2f(m_right - m_screeBorder, m_bottom + m_screeBorder) + Vector2f(m_zoomFactor * (m_position[0] + m_focusPointX) - m_focusPointX, m_zoomFactor * (m_position[1] + m_focusPointY) - m_focusPointY);

	int rowMin, rowMax, colMin, colMax;
	Math::isometricToCol(m_cullingVertices[0][0] - m_enlargeBorder, m_cullingVertices[0][1] - m_enlargeBorder, colMax, m_cellHeight * m_zoomFactor, 0, 128);
	Math::isometricToRow(m_cullingVertices[3][0] + m_enlargeBorder, m_cullingVertices[3][1] - m_enlargeBorder, rowMax, m_cellWidth * m_zoomFactor, 0, 128);
	Math::isometricToRow(m_cullingVertices[1][0], m_cullingVertices[1][1], rowMin, m_cellWidth * m_zoomFactor, 0, 128);
	Math::isometricToCol(m_cullingVertices[2][0], m_cullingVertices[2][1], colMin, m_cellHeight * m_zoomFactor, 0, 128);

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

void Map::drawCullingRect() {
	Vector3f m_position = camera.getPosition();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(m_focusPointX - m_zoomFactor * (m_position[0] + m_focusPointX), m_focusPointY - m_zoomFactor * (m_position[1] + m_focusPointY), 0.0f);

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);

	glVertex3f(m_cullingVertices[0][0], m_cullingVertices[0][1], 0.0f);
	glVertex3f(m_cullingVertices[1][0], m_cullingVertices[1][1], 0.0f);
	glVertex3f(m_cullingVertices[2][0], m_cullingVertices[2][1], 0.0f);
	glVertex3f(m_cullingVertices[3][0], m_cullingVertices[3][1], 0.0f);

	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Map::drawMouseRect(float mouseX, float curMouseX, float mouseY, float curMouseY) {
	
	if (m_selectionMode == Enums::SelectionMode::MARKER || m_selectionMode == Enums::SelectionMode::RASTERIZER ) return;
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(2.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	

	if (m_selectionMode == Enums::SelectionMode::BOXSELECTION || m_selectionMode == Enums::SelectionMode::NONE) {
		float left = std::min(mouseX, curMouseX);
		float bottom = std::min(mouseY, curMouseY);
		float top = std::max(mouseY, curMouseY);
		float right = std::max(mouseX, curMouseX);

		glBegin(GL_QUADS);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(left, bottom, 0.0f);
		glVertex3f(left, top, 0.0f);
		glVertex3f(right, top, 0.0f);
		glVertex3f(right, bottom, 0.0f);
		glEnd();

	}else if (m_selectionMode == Enums::SelectionMode::ISOSELECTION) {

		float offsetX = m_zoomFactor * (camera.getPositionX() + m_focusPointX) - m_focusPointX;
		float offsetY = m_zoomFactor * (camera.getPositionY() + m_focusPointY) - m_focusPointY;

		glLoadMatrixf(&camera.getViewMatrix()[0][0]);
		int row1, col1, row2, col2;
		float p00, p01, p10, p11;
		if (m_discreteSelection) {
			Math::isometricToCartesian(mouseX + offsetX, mouseY + offsetY, row1, col1, m_cellWidth  * m_zoomFactor, m_cellHeight * m_zoomFactor);
			Math::isometricToCartesian(curMouseX + offsetX, curMouseY + offsetY, row2, col2, m_cellWidth  * m_zoomFactor, m_cellHeight * m_zoomFactor);
		}else {
			p00 = mouseX + offsetX;
			p01 = mouseY + offsetY;
			p10 = curMouseX + offsetX;
			p11 = curMouseY + offsetY;
			Math::isometricToCartesian(p00, p01, m_cellWidth  * m_zoomFactor, m_cellHeight * m_zoomFactor);
			Math::isometricToCartesian(p10, p11, m_cellWidth  * m_zoomFactor, m_cellHeight * m_zoomFactor);
		}

		float rowMin = m_discreteSelection ? static_cast<float>(std::min(row1, row2)) : std::min(p00, p10);
		float rowMax = m_discreteSelection ? static_cast<float>(std::max(row1, row2)) : std::max(p00, p10);
		float colMin = m_discreteSelection ? static_cast<float>(std::min(col1, col2) - 1) : std::min(p01, p11);
		float colMax = m_discreteSelection ? static_cast<float>(std::max(col1, col2)) : std::max(p01, p11);

		glBegin(GL_QUADS);
		glColor3f(0.0f, 1.0f, 0.0f);

		float cartX = m_discreteSelection ? rowMax + 0.5f : rowMax + 1.0f;
		float cartY = m_discreteSelection ? colMax - 0.5f : colMax + 1.0f;
		Math::cartesianToIsometric(cartX, cartY, m_cellWidth, m_cellHeight);
		glVertex3f(cartX * m_zoomFactor + (camera.getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor), -cartY * m_zoomFactor + (camera.getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor), 0.0f);

		cartX = m_discreteSelection ? rowMin - 0.5f : rowMin - 1.0f;
		cartY = m_discreteSelection ? colMax - 0.5f : colMax + 1.0f;
		Math::cartesianToIsometric(cartX, cartY, m_cellWidth, m_cellHeight);
		glVertex3f(cartX * m_zoomFactor + (camera.getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor), -cartY * m_zoomFactor + (camera.getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor), 0.0f);

		cartX = m_discreteSelection ? rowMin - 0.5f : rowMin - 1.0f;
		cartY = m_discreteSelection ? colMin - 0.5f : colMin - 1.0f;
		Math::cartesianToIsometric(cartX, cartY, m_cellWidth, m_cellHeight);
		glVertex3f(cartX * m_zoomFactor + (camera.getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor), -cartY * m_zoomFactor + (camera.getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor), 0.0f);

		cartX = m_discreteSelection ? rowMax + 0.5f : rowMax + 1.0f;
		cartY = m_discreteSelection ? colMin - 0.5f : colMin - 1.0f;
		Math::cartesianToIsometric(cartX, cartY, m_cellWidth, m_cellHeight);
		glVertex3f(cartX * m_zoomFactor + (camera.getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor), -cartY * m_zoomFactor + (camera.getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor), 0.0f);
		glEnd();
	}


	glLineWidth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Map::skipFileKey(std::ifstream & read) {
	read.ignore(std::numeric_limits<std::streamsize>::max(), ':');
	while (read.peek() == ' ' || read.peek() == '\t')
		read.ignore();
}

void Map::loadCollision(std::string name) {
	std::ifstream read(name);

	read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	char buffer[128];
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

			defaultAABBList.push_back({ static_cast<float>(width) , static_cast<float>(height) , static_cast<float>(xOffset) , static_cast<float>(yOffset) });
		}

		read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');		// skip the rest of the line
		if (read.peek() == '#') {
			read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');	// skip the renderBlockSize rules comment
			readState = LOADING_DEFAULT_RENDERBLOCKS;
		}
	}


	while (readState == LOADING_DEFAULT_RENDERBLOCKS) {
		skipFileKey(read);													// rbSize text
		unsigned int width = 0;
		unsigned int height = 0;
		unsigned int depth = 0;
		read >> width >> height >> depth;

		defaultRenderBlockSizes.push_back({ width, height, depth });
		read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');		// skip the rest of the line
		if (read.peek() == '#') {
			read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');	// begin tile type definitions comment
			skipFileKey(read);												// num_tiles text
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
			colAndBlockId.push_back({ colliderType, renderBlockType });
			read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	read.close();
}

void Map::loadMap(std::string name) {
	std::ifstream read(name);
	char buffer[128];
	memset(buffer, 0, sizeof(buffer));

	m_numColumns = 0;
	m_numRows = 0;
	m_cellWidth = 0;
	m_cellHeight = 0;
	m_numLayers = 0;
	int row = 0;
	int column = 0;

	while (read.peek() == '#')
		read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	for (int i = 0; i < 6; ++i) {
		skipFileKey(read);
		switch (i) {
		case 0: read >> m_numColumns; break;
		case 1: read >> m_numRows;	break;
		case 2: read >> m_cellWidth; break;
		case 3: read >> m_cellHeight; break;
		case 4: read >> m_numLayers;	break;
		case 5: read.getline(buffer, sizeof(buffer), '\n'); break;		// tileSet filename
		}

		read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');		// skip the rest of the line (BUGFIX: and the # begin layer def comment line)
	}

	m_layer.resize(m_numLayers);

	unsigned int layer = 0;
	read.ignore(std::numeric_limits<std::streamsize>::max(), '{');			// ignore up past "Layers {"
	read.ignore(1, '\n');													// ignore the '\n' past '{'

	while (read.peek() != '}') {
		row = 0;
		column = 0;
		size_t tallestRenderBlock = 0;
		read.ignore(std::numeric_limits<std::streamsize>::max(), '{');			// ignore up past "layer_# {"
		read.ignore(1, '\n');													// ignore the '\n' past '{'
		m_layer[layer] = new std::pair<int, unsigned int>*[m_numColumns];
		for (int i = 0; i < m_numColumns; ++i)
			m_layer[layer][i] = new std::pair<int, unsigned int>[m_numRows];
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
				Math::cartesianToIsometric(cartX, cartY, m_cellWidth, m_cellHeight);
				m_cells.push_back({ rect, cartX, -cartY, false, true,  collRect , blockRect, colAndBlockId[m_layer[layer][column][row].first][0] != 0 });
				m_layer[layer][column][row].second = m_cells.size() - 1;
			}

			if (read.peek() == '\n') {
				read.ignore(1, '\n');
				row = 0;
				column++;
			}else if (read.peek() == ',') {
				read.ignore(1, ',');
				row++;
				if (row >= m_numRows) {
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

bool Map::isValid(const int row, const int column) const {
	return (row >= 0 && row < 128 && column >= 0 && column < 128);
}

void Map::setSelectionMode(Enums::SelectionMode selectionMode) {
	m_selectionMode = selectionMode;
}

const int Map::getCellWidth() const {
	return m_cellWidth;
}

const int Map::getCellHeight() const {
	return m_cellHeight;
}

const std::vector<Cell>& Map::getCollisionCells() const {
	return m_collisionCells;
}

bool& Map::useCulling() {
	return m_useCulling;
}

bool& Map::discreteSelection() {
	return m_discreteSelection;
}

float& Map::screenBorder() {
	return m_screeBorder;
}

float& Map::enlargeBorder() {
	return m_enlargeBorder;
}

bool Map::FindSingleCell(SingleSelectedCell const& s1, SingleSelectedCell const& s2) {
	return s1.row == s2.row && s1.col == s2.col;
}