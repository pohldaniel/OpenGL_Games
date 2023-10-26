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

struct Cell2 {
	const TextureRect& rect;
	float posX;
	float posY;
	bool selected;
	bool visible;
	const Vector4f& collisionRect;
	const std::array<unsigned int, 3>& renderBlock;
	bool hasCollision;
};

struct SingleSelectedCell2 {
	int row;
	int col;
	bool found;
};

enum SelectionMode2 {
	BOXSELECTION,
	ISOSELECTION,
	MARKER,
	RASTERIZER,
	ENTITY
};

class Map : public MouseEventListener, public KeyboardEventListener {

public:

	Map(const Camera& camera);
	~Map();
	
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;

	void render();
	void resize(int deltaW, int deltaH);

	void loadMap(std::string name);


	void processCache(std::vector<std::reference_wrapper<Cell2>>& cache, bool visible, bool selected, bool clear);
	void processCache(std::vector<std::reference_wrapper<Cell2>>& cache, bool visible, bool selected, std::vector<std::reference_wrapper<Cell2>>& storage, bool clearAfterCopy);
	void culling();
	void drawCullingRect();

	bool isValid(const int row, const int column) const;

	void SkipFileKey(std::ifstream & read);

	bool m_useCulling = true;
	bool m_drawCullingRect = false;
	float m_left, m_right, m_bottom, m_top;
	float m_screeBorder = 0.0f;
	float m_enlargeBorder = 100.0f;
	unsigned int m_atlas;

	std::vector<std::pair<int, unsigned int>**> m_layer;

	int numColumns = 0;
	int numRows = 0;
	int cellWidth = 0;
	int cellHeight = 0;
	int numLayers = 0;

	std::vector<Cell2> m_cells;
	std::vector<Cell2> m_visibleCells;
	std::vector<Cell2> m_collisionCells;

	std::vector<std::reference_wrapper<Cell2>> m_selectedCells;
	std::vector<std::reference_wrapper<Cell2>> m_cellCache;
	std::vector<SingleSelectedCell2> m_singleCache;
	std::array<Vector2f, 4> m_cullingVertices;

	bool m_mouseDown = false;
	bool m_mouseMove = false;
	bool m_redrawMap = true;
	float m_mouseX, m_mouseY;
	float m_curMouseX, m_curMouseY;

	std::vector<std::array<unsigned int, 3>> defaultRenderBlockSizes;
	std::vector<Vector4f> defaultAABBList;
	std::vector<std::array<int, 2>> colAndBlockId;

	const Camera& camera;
	float m_zoomFactor = 1.0;
	float m_focusPointX;
	float m_focusPointY;
	

	SelectionMode2 m_selectionMode = SelectionMode2::ENTITY;

	static bool FindSingleCell(SingleSelectedCell2 const& s1, SingleSelectedCell2 const& s2);
};