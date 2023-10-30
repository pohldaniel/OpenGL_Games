#pragma once
#pragma once

#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <sstream>

#include <engine/input/EventDispatcher.h>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Rect.h>
#include <engine/Vector.h>
#include <engine/Camera.h>

#include "TileSet.h"
#include "Enums.h"

struct Cell {
	const TextureRect& rect;
	float posX;
	float posY;
	bool selected;
	bool visible;
	const Vector4f& collisionRect;
	const std::array<unsigned int, 3>& renderBlock;
	bool hasCollision;
};

struct SingleSelectedCell {
	int row;
	int col;
	bool found;
};

class Map : public MouseEventListener, public KeyboardEventListener {

public:

	Map(const Camera& camera);
	~Map();
	
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;

	void drawRaw();
	void drawCullingRect();
	void drawMouseRect(float mouseX, float curMouseX, float mouseY, float curMouseY);
	void resize(int deltaW, int deltaH);

	void loadCollision(std::string name);
	void loadMap(std::string name);

	bool isValid(const int row, const int column) const;

	const int getCellWidth() const;
	const int getCellHeight() const;
	const std::vector<Cell>& getCollisionCells() const;
	void setSelectionMode(Enums::SelectionMode selectionMode);
	void setZoomFactor(float zoomFactor);

	bool& useCulling();
	bool& discreteSelection();
	bool& showCullingRect();
	float& screenBorder();
	float& enlargeBorder();

private:

	void processCache(std::vector<std::reference_wrapper<Cell>>& cache, bool visible, bool selected, bool clear);
	void processCache(std::vector<std::reference_wrapper<Cell>>& cache, bool visible, bool selected, std::vector<std::reference_wrapper<Cell>>& storage, bool clearAfterCopy);
	void culling();
	
	void skipFileKey(std::ifstream & read);
	
	float m_left, m_right, m_bottom, m_top;
	float m_screeBorder = 0.0f;
	float m_enlargeBorder = 100.0f;
	bool m_discreteSelection = true;
	bool m_useCulling = true;
	bool m_showCullingRect = false;

	unsigned int m_atlas;
	
	int m_numColumns = 0;
	int m_numRows = 0;
	int m_cellWidth = 0;
	int m_cellHeight = 0;
	int m_numLayers = 0;

	std::vector<std::pair<int, unsigned int>**> m_layer;
	std::vector<Cell> m_cells;
	std::vector<Cell> m_visibleCells;
	std::vector<Cell> m_collisionCells;

	std::vector<std::reference_wrapper<Cell>> m_selectedCells;
	std::vector<std::reference_wrapper<Cell>> m_cellCache;
	std::vector<SingleSelectedCell> m_singleCache;
	std::array<Vector2f, 4> m_cullingVertices;

	bool m_mouseDown = false;
	bool m_mouseMove = false;

	float m_mouseX, m_mouseY;
	float m_curMouseX, m_curMouseY;

	std::vector<std::array<unsigned int, 3>> defaultRenderBlockSizes;
	std::vector<Vector4f> defaultAABBList;
	std::vector<std::array<int, 2>> colAndBlockId;

	const Camera& camera;
	float m_zoomFactor = 1.0;
	float m_focusPointX;
	float m_focusPointY;
	
	Enums::SelectionMode m_selectionMode = Enums::SelectionMode::NONE;

	static bool FindSingleCell(SingleSelectedCell const& s1, SingleSelectedCell const& s2);
};