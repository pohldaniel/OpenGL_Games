#pragma once

#include <fstream>
#include <sstream>
#include <memory>

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/TrackBall.h>
#include <engine/Clock.h>
#include <engine/Pixelbuffer.h>
#include <engine/Framebuffer.h>
#include <engine/Spritesheet.h>
#include <engine/ZoomableQuad.h>

#include "StateMachine.h"
#include "Background.h"
#include "TileSet.h"
#include "Enums.h"
#include "Animation.h"

#include "Math.hpp"


#include "Entity.h"
#include "AnimationController.h"
#include "Prefab.h"

struct Cell {
	const TextureRect& rect;
	float posX;
	float posY;
	bool selected;
	bool visible;
	const Vector4f& collisionRect;
	const std::array<unsigned int, 3>& renderBlock;
	bool hasCollision;
	int row;
	int col;
};

struct SingleSelectedCell {
	int row;
	int col;
	bool found;
};

enum SelectionMode {
	BOXSELECTION,
	ISOSELECTION,
	MARKER,
	RASTERIZER
};

enum SelectedEntity {
	HERO,
	ARCHER
};

class Game : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Game(StateMachine& machine);
	~Game();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void OnKeyUp(Event::KeyboardEvent& event) override;

private:

	void loadMap(std::string name);
	void loadCollision(std::string name);
	void SkipFileKey(std::ifstream & read);

	void applyTransformation(TrackBall& arc);
	void renderUi();
	void culling();
	void drawCullingRect();
	void drawMouseRect();

	void processCache(std::vector<std::reference_wrapper<Cell>>& cache, bool visible, bool selected, bool clear);
	void processCache(std::vector<std::reference_wrapper<Cell>>& cache, bool visible, bool selected, std::vector<std::reference_wrapper<Cell>>& storage, bool clearAfterCopy);

	bool isValid(const int row, const int column) const;

	Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_useCulling = true;
	bool m_drawCullingRect = false;
	float m_left, m_right, m_bottom, m_top;
	float m_screeBorder = 0.0f;
	float m_zoomFactor = 1.0f;
	float m_focusPointY;
	float m_focusPointX;
	float m_enlargeBorder = 100.0f;
	bool m_discreteSelection = true;
	bool m_autoRedraw = false;


	Background m_background;
	ZoomableQuad m_zoomableQuad;
	Framebuffer m_mainRT;

	unsigned int m_atlas;

	std::vector<std::pair<int, unsigned int>**> m_layer;

	int numColumns = 0;
	int numRows = 0;
	int cellWidth = 0;
	int cellHeight = 0;
	int numLayers = 0;

	std::vector<Cell> m_cells;
	std::vector<Cell> m_visibleCells;
	std::vector<Cell> m_collisionCells;

	std::vector<std::reference_wrapper<Cell>> m_selectedCells;
	std::vector<std::reference_wrapper<Cell>> m_cellCache;
	std::vector<SingleSelectedCell> m_singleCache;

	std::vector<Entity> m_entities;

	bool move;
	std::array<Vector2f, 4> m_cullingVertices;

	bool m_mouseDown = false;
	bool m_mouseMove = false;
	bool m_redrawMap = true;
	float m_mouseX, m_mouseY;
	float m_curMouseX, m_curMouseY;
	SelectionMode m_selectionMode = SelectionMode::ISOSELECTION;

	static bool FindSingleCell(SingleSelectedCell const& s1, SingleSelectedCell const& s2);

	
	
	SelectedEntity m_selctedEntity = SelectedEntity::ARCHER;


	std::vector<std::array<unsigned int, 3>> defaultRenderBlockSizes;
	std::vector<Vector4f> defaultAABBList;

	std::vector<std::array<int, 2>> colAndBlockId;

	void drawIsometricRect(float posX, float posY, Vector4f sizeOffset);
};