#pragma once

#include <fstream>
#include <sstream>

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
#include "Tile.h"
#include "Map.h"
#include "Hex.h"

/*auto hexhash = [](const Hex& h) {
	//return (size_t)(pt.x*100 + pt.y);
	//hash<int> int_hash;
	size_t hq = h.get_q();
	size_t hr = h.get_r();
	return hq ^ (hr + 0x9e3779b9 + (hq << 6) + (hq >> 2));
};

auto hexequal = [](const Hex& hex1, const Hex& hex2) {
	return ((hex1.get_q() == hex2.get_q()) && (hex1.get_r() == hex2.get_r()));
};*/

struct hex_hash {

	std::size_t operator () (const Hex& h) const {
		size_t hq = h.get_q();
		size_t hr = h.get_r();
		return hq ^ (hr + 0x9e3779b9 + (hq << 6) + (hq >> 2));
	}
};

struct hex_equal {
	bool operator() (Hex const& hex1, Hex const& hex2) const{
		return ((hex1.get_q() == hex2.get_q()) && (hex1.get_r() == hex2.get_r()));
	}
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
	void updateCamera(float delta);
	void setLimits(int l, int r, int t, int b);
	void CenterCameraOverCell(int row, int col);
	void CenterCameraToPoint(int x, int y);
	void CenterToPoint(int x, int y);
	Vector2f GetCellPosition(unsigned int r, unsigned int c) const;
	Vector2f GetCellPosition(unsigned int index) const;
	void createBoard(int nr);

	Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;

	bool m_initUi = true;
	bool m_drawUi = false;
	bool m_debugPhysic = false;
	
	Background m_background;

	Spritesheet* m_spriteSheetIso;
	Spritesheet* m_spriteSheetHex;

	Shader* m_shader;
	Shader* m_shaderArray;
	Shader* m_shaderLevel;

	float dist = 4.7f;
	float scale = 0.65f;
	Matrix4f m_projection = Matrix4f::IDENTITY;

	std::vector<Tile> isoTiles;
	std::vector<Tile> hexTiles;
	std::list<Hex> hexlist;

	//std::unordered_map<Hex, int>::size_type n = 10;
	//std::unordered_map<Hex, int, std::function<size_t(const Hex&)>,std::function<bool(Hex const&, Hex const&)>> heights(n, hex_hash, hex_equal);

	std::unordered_map<Hex, int, hex_hash, hex_equal> heights;

	// HexTile to Value
	//using HexMap = std::unordered_map<Hex, int, decltype(hexhash), decltype(hexequal)>;
	//HexMap heights{ HexMap(10,hexhash,hexequal) };

	float mSpeed = 400.0f;

	int mDirX = 0;
	int mDirY = 0;
	int mLimitL;
	int mLimitR;
	int mLimitT;
	int mLimitB;

	bool mKeyScrollX = false;
	bool mKeyScrollY = false;
	bool mMouseScrollX = false;
	bool mMouseScrollY = false;

	int m_cols;
	int m_rows;

	const int ISO_WIDTH;
	const int ISO_HEIGHT;

	const int HEX_WIDTH;
	const int HEX_HEIGHT;

	const int HEX_OFFSET_X; //out of gimp
	const int HEX_OFFSET_Y; //out of gimp

	Map m_isoMap;
	Map m_hexMap;

	unsigned int** m_tileId;
};
