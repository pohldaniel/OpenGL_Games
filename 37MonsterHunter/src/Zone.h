#pragma once

#include <vector>
#include <engine/Vector.h>
#include <engine/Camera.h>

struct Cell {
	float posX;
	float posY;
	int currentFrame;
	float centerX;
	float centerY;
	float height;
};

//Objects
//Entitis
//Collision Layer

struct CellShadow : public Cell {

	CellShadow(float posX, float posY, int currentFrame, float centerX, float centerY, float height, bool hasShadow):
		posX(posX), posY(posY), currentFrame(currentFrame), centerX(centerX), centerY(centerY), height(height), hasShadow(hasShadow){

	}

	float posX;
	float posY;
	int currentFrame;
	float centerX;
	float centerY;
	float height;
	bool hasShadow;
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

class Zone {

	static const int MAX_POINTS = 1000;

public:
	Zone(const Camera& camera);
	~Zone();

	void update(float dt);
	void draw();
	void resize();
	void loadZone(const std::string& path);
	void initDebug();
	void updateBorder();
	CellShadow& getPlayer();
	const std::vector<Rect>& getCollisionRects();
	float getMapHeight();

	void setUseCulling(bool useCulling);
	void setScreeBorder(float screeBorder);
	void setDrawScreenBorder(bool drawScreenBorder);
	void setDrawCenter(bool drawCenter);
	void setDebugCollision(bool debugCollision);

private:

	void updatePoints();
	void culling();
	void drawCullingRect();
	int posYToRow(float y, float cellHeight, int min, int max, int shift);
	int posXToCol(float y, float cellHeight, int min, int max, int shift);
	bool isRectOnScreen(float posX, float posY, float width, float height);

	std::vector<std::pair<int, unsigned int>**> m_layers;
	std::vector<AnimatedCell> m_animatedCells;
	std::vector<Cell> m_cellsBackground;
	std::vector<CellShadow> m_cellsMain;

	std::vector<AnimatedCell> m_visibleCellsAni;
	std::vector<Cell> m_visibleCellsBG;
	std::vector<CellShadow> m_visibleCellsMain;

	std::vector<Rect> m_collisionRects;

	float m_mapHeight, m_tileHeight, m_tileWidth;
	float m_left, m_right, m_bottom, m_top;
	float m_screeBorder;
	bool m_useCulling, m_drawScreenBorder, m_drawCenter, m_debugCollision;
	bool m_borderDirty;
	std::array<Vector2f, 4> m_cullingVertices;
	size_t m_playerIndex, m_cols, m_rows;

	unsigned int m_vao, m_vbo;
	Vector3f *pointBatch, *pointBatchPtr;
	uint32_t m_pointCount;

	const Camera& camera;

	float elapsedTime;
	int currentFrame;
	int frameCount;
};