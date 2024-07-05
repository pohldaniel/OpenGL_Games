#pragma once

#include <vector>
#include <array>
#include <engine/Vector.h>
#include <engine/Camera.h>
#include <engine/Rect.h>

class SpriteEntity;
class Player;
class Character;

struct Cell {
	float posX;
	float posY;
	int currentFrame;
	float centerX;
	float centerY;
	float height;
	bool visibile;
};

struct CellShadow : public Cell {

	CellShadow(float _posX, float _posY, int _currentFrame, float _centerX, float _centerY, float _height, bool _visibile, bool hasShadow) : hasShadow(hasShadow) {
		posX = _posX;
		posY = _posY;
		currentFrame = _currentFrame;
		centerX = _centerX;
		centerY = _centerY;
		height = _height;
		visibile = _visibile;
	}
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
	void loadZone(const std::string& path, size_t capacity);
	void initDebug();
	void updateBorder();
	Player& getPlayer();

	const std::vector<Rect>& getCollisionRects();
	const std::vector<std::unique_ptr<SpriteEntity>>& getSpriteEntities();
	const std::vector<std::reference_wrapper<Character>>& getCharacters();
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

	std::vector<std::unique_ptr<SpriteEntity>> m_spriteEntities;
	std::vector<std::reference_wrapper<Character>> m_characters;

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