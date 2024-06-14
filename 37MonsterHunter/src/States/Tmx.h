#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/Background.h>
#include <engine/Texture.h>
#include <engine/Rect.h>
#include <States/StateMachine.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>

enum Sorting {
	NONE,
	TOP
};

struct Cell {
	float posX;
	float posY;
	int currentFrame;
	//Sorting sortKey;
	float centerX;
	float centerY;
	float height;
};

struct AnimatedCell {
	float posX;
	float posY;
	int currentFrame;
	int startFrame;
	int frameCount;
	float elapsedTime;
	float updateTime;
};

struct PointVertex {
	Vector3f position;
};

class Tmx : public State, public MouseEventListener, public KeyboardEventListener {

	static const int MAX_PARTICLES = 1000;

public:

	Tmx(StateMachine& machine);
	~Tmx();

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

	void renderUi();
	void loadMap(const std::string& path);
	void updatePoints();

	bool m_initUi = true;
	bool m_drawUi = false;
	bool m_drawCenter = false;
	Camera m_camera;

	std::vector<unsigned> m_tileTextures;
	std::vector<std::pair<int, unsigned int>**> m_layers;
	std::vector<AnimatedCell> m_animatedCells;
	std::vector<Cell> m_cellsBackground;
	std::vector<Cell> m_cellsMain;
	std::vector<Cell> m_cellsTop;
	std::vector<int> m_indexArray;

	unsigned int m_atlasWorld;
	unsigned int m_vao, m_vbo;
	float m_mapHeight, m_tileHeight, m_tileWidth;
	float m_movingSpeed;
	float m_viewWidth;
	float m_viewHeight;
	PointVertex* pointBatch;
	PointVertex* pointBatchPtr;
	uint32_t m_pointCount = 0;

	size_t m_playerIndex;
};