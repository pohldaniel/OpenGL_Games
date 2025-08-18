#pragma once

#include<string>
#include <unordered_map>
#include <engine/Camera.h>
#include <engine/Rect.h>
#include <engine/TileSet.h>

struct Cell {
	float posX;
	float posY;
	float width;
	float height;
	int currentFrame;
	float centerX;
	float centerY;
	bool visibile;
	bool flipped;
};

struct CellShadow : public Cell {

	CellShadow(float _posX, float _posY, float _width, float _height, int _currentFrame, float _centerX, float _centerY, bool _visibile, bool _flipped, bool isNoticed, bool hasShadow) : hasShadow(hasShadow), isNoticed(isNoticed) {
		posX = _posX;
		posY = _posY;
		width = _width;
		height = _height;
		currentFrame = _currentFrame;
		centerX = _centerX;
		centerY = _centerY;
		visibile = _visibile;
		flipped = _flipped;
	}
	bool hasShadow;
	bool isNoticed;
};

struct TileSetData {
	std::vector<std::pair<std::string, float>> pathSizes;
	std::vector<std::pair<std::string, unsigned int>> offsets;
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
	void loadZone(const std::string path, const std::string currentTileset);
	void loadTileSet(const std::vector<std::pair<std::string, float>>& pathSizes, const std::vector<std::pair<std::string, unsigned int>>& offsets);
	void loadTileSetData(const std::string& path);
	void culling();
	void updateBorder();
	int posYToRow(float y, float cellHeight, int min, int max, int shift);
	int posXToCol(float y, float cellHeight, int min, int max, int shift);
	bool isRectOnScreen(float posX, float posY, float width, float height);
	void updatePoints();
	void initDebug();

	bool m_useCulling;
	float m_screeBorder;
	std::array<Vector2f, 4> m_cullingVertices;
	std::vector<std::pair<int, unsigned int>**> m_layers;
	bool m_borderDirty;
	size_t m_playerIndex, m_cols, m_rows;
	std::vector<Cell> m_cellsBackground;
	std::vector<CellShadow> m_cellsMain;
	std::vector<Rect> m_collisionRects;
	TileSet m_tileSet;
	std::unordered_map<std::string, unsigned int> m_charachterOffsets;
	unsigned int m_spritesheet;
	std::string m_currentTileset;
	const Camera& camera;
	static std::unordered_map<std::string, TileSetData> TileSets;
	float m_left, m_right, m_bottom, m_top;
	float m_mapHeight, m_tileHeight, m_tileWidth;

	std::vector<Cell> m_visibleCellsBackground;
	std::vector<CellShadow> m_visibleCellsMain;

	unsigned int m_vao, m_vbo;
	Vector3f *pointBatch, *pointBatchPtr;
	uint32_t m_pointCount;
};