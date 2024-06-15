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

struct Cell {
	float posX;
	float posY;
	int currentFrame;
	float centerX;
	float centerY;
	float height;
};

struct AnimatedCell {
	float posX;
	float posY;
	int currentFrame;
	int startFrame;
};

struct PointVertex {
	Vector3f position;
};

class Tmx : public State, public MouseEventListener, public KeyboardEventListener {

	static const int MAX_POINTS = 1000;

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
	void culling();
	void drawCullingRect();
	int posYToRow(float y, float cellHeight, int min, int max, int shift);
	int posXToCol(float y, float cellHeight, int min, int max, int shift);
	bool isRectOnScreen(float posX, float posY, float width, float height);

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_drawCenter = false;
	bool m_useCulling = true;
	bool m_drawScreenBorder = false;
	Camera m_camera;

	std::vector<unsigned> m_tileTextures;
	std::vector<std::pair<int, unsigned int>**> m_layers;
	std::vector<AnimatedCell> m_animatedCells;
	std::vector<Cell> m_cellsBackground;
	std::vector<Cell> m_cellsMain;

	std::vector<AnimatedCell> m_visibleCellsAni;
	std::vector<Cell> m_visibleCellsBG;
	std::vector<Cell> m_visibleCellsMain;

	unsigned int m_atlasWorld;
	unsigned int m_vao, m_vbo;
	float m_mapHeight, m_tileHeight, m_tileWidth;
	float m_movingSpeed;
	float m_viewWidth;
	float m_viewHeight;
	float m_left, m_right, m_bottom, m_top;
	float m_screeBorder = 0.0f;

	PointVertex* pointBatch;
	PointVertex* pointBatchPtr;
	uint32_t m_pointCount = 0;
	std::array<Vector2f, 4> m_cullingVertices;
	size_t m_playerIndex;


	float elapsedTime;
	int currentFrame;
	int frameCount;
};