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

#include "Camera.h"
#include "Map.h"
#include "Tile.h"

struct Cell {
	const TextureRect& rect;	
	float posX;
	float posY;
	bool selected;
	bool visible;
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

	void applyTransformation(TrackBall& arc);
	void renderUi();
	

	Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_useCulling = true;
	bool m_drawCullingRect = false;

	Background m_background;
	ZoomableQuad m_zoomableQuad;
	Framebuffer m_mainRT;

	std::vector<TextureRect> m_textureRects;
	unsigned int m_atlas;

	void loadTileSet(std::string name);
	void loadMap(std::string name);
	void SkipFileKey(std::ifstream & read);

	std::vector<std::pair<int, unsigned int>**> m_layer;

	int numColumns = 0;
	int numRows = 0;
	int cellWidth = 0;
	int cellHeight = 0;
	int numLayers = 0;

	std::vector<Cell> m_cells;
	std::vector<std::reference_wrapper<Cell>> m_selectedCells;

	//std::vector<std::vector<std::reference_wrapper<Cell>>> m_selectedChunks;

	std::vector<Cell> m_visibleCells;
	bool move;
	//int m_chunk = -1;
	//bool m_pushChunk = true;
	void culling();
	void drawCullingRect();
	void drawMouseRect();
	void unselect(bool visible, bool selected, bool clear, int chunk = -1);
	void resetAllChunks();


	float m_left, m_right, m_bottom, m_top;
	float m_screeBorder = 0.0f;
	float m_zoomFactor = 1.0f;
	float m_focusPointY;
	float m_focusPointX;
	float m_enlargeBorder = 100.0f;

	void cartesianToIsometric(float & x, float & y, float cellWidth = 32.0f, float cellHeight = 32.0f);
	void isometricToCartesian(float& x, float& y, float cellWidth = 32.0f, float cellHeight = 32.0f);
	void isometricToCartesian(float x, float y, int& row, int& col, float cellWidth = 32.0f, float cellHeight = 32.0f);
	void isometricToCartesian(float x, float y, int& row, int& col, float cellWidth, float cellHeight, int min, int max);

	void isometricToRow(float x, float y, int& row, float cellWidth = 32.0f);
	void isometricToRow(float x, float y, int& row, float cellWidth, int min, int max);

	void isometricToCol(float x, float y, int& col, float cellHeight = 32.0f);
	void isometricToCol(float x, float y, int& col, float cellHeight, int min, int max);

	bool isValid(const int row, const int column) const;
	std::array<Vector2f, 4> corners;

	bool m_mouseDown = false;
	float m_mouseX, m_mouseY;
	float m_curMouseX, m_curMouseY;
	int m_rowMin, m_rowMax, m_colMin, m_colMax;
};
