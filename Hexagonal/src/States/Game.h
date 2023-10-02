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

#include "StateMachine.h"
#include "Background.h"
#include "TileSet.h"

#include "Camera.h"
#include "Map.h"
#include "Tile.h"

struct Cell {

	//Cell()

	const TextureRect& rect;
	//float boundX;
	//float boundY;
	float posX;
	float posY;

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
	Pixelbuffer m_pixelbuffer;
	Texture m_texture;

	std::vector<TextureRect> m_textureRects;
	unsigned int m_atlas;

	void loadTileSet(std::string name);
	void loadMap(std::string name);
	void SkipFileKey(std::ifstream & read);

	std::vector<unsigned int**> m_layer;

	int numColumns = 0;
	int numRows = 0;
	int cellWidth = 0;
	int cellHeight = 0;
	int numLayers = 0;

	std::vector<Cell> m_cells;
	std::vector<Cell> m_visibleCells;
	bool move;

	void culling();
	void drawCullingRect();

	float m_left, m_right, m_bottom, m_top;
	float m_screeBorder = 0.0f;

	void cartesianToIartesian(float & x, float & y, float cellWidth = 32.0f, float cellHeight = 32.0f);
	void isometricToCartesian(float& x, float& y, float cellWidth = 32.0f, float cellHeight = 32.0f);
	void isometricToCartesian(float x, float y, int& row, int& col, float cellWidth = 32.0f, float cellHeight = 32.0f);
	void isometricToRow(float x, float y, int& row, float cellWidth = 32.0f);
	void isometricToCol(float x, float y, int& col, float cellHeight = 32.0f);
	bool isValid(const int row, const int column) const;
	std::array<Vector2f, 4> corners;
};
