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

struct Tile {
	Vector2f position;
	Vector2f size;
	unsigned int gid;
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
	void createBuffer();
	void addTile(const Tile tile, std::vector<float>& vertices, std::vector<unsigned int>& indices, std::vector<unsigned int>& mapIndices);
	void update2(float delta);
	void setLimits(int l, int r, int t, int b);
	void CenterCameraOverCell(int row, int col);
	void CenterCameraToPoint(int x, int y);
	void CenterToPoint(int x, int y);
	Vector2f GetCellPosition(unsigned int r, unsigned int c) const;
	Vector2f GetCellPosition(unsigned int index) const;
	void setOrigin(const float x, const float y);
	void setOrigin(const Vector2f &origin);
	void updateTilePositions();

	Vector2f m_origin;
	Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;

	bool m_initUi = true;
	bool m_drawUi = false;
	bool m_debugPhysic = false;
	
	Background m_background;

	std::vector<Quad*> m_quads;
	std::vector<Quad*> m_quads2;

	Spritesheet* m_spriteSheet;
	Spritesheet* m_spriteSheet2;

	Shader* m_shader;
	Shader* m_shaderArray;
	Shader* m_shaderLevel;

	float dist = 4.7f;
	float scale = 0.65f;
	Matrix4f m_projection = Matrix4f::IDENTITY;

	std::vector<Tile> tiles;
	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indexBuffer;
	std::vector<unsigned int> m_indexMap;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_vboMap = 0;
	unsigned int m_ibo = 0;

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
};
