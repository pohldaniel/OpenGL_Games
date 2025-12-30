#pragma once

#include <string>
#include <unordered_map>
#include <engine/Camera.h>
#include <engine/TileSet.h>
#include <engine/Framebuffer.h>

#include "DataStructs.h"

class Zone {

	static const int MAX_POINTS = 1000;

public:

	Zone(const Camera& camera, const bool initDebug = true);
	virtual ~Zone();

	virtual void draw() = 0;
	virtual void update(float dt);
	virtual void resize();
	
	virtual void loadZone(const std::string path, const std::string currentTileset) = 0;
	
	virtual void setDrawCenter(bool drawCenter);
	virtual void setUseCulling(bool useCulling);
	virtual void setDebugCollision(bool debugCollision);


	static void LoadTileSetData(const std::string& path);

protected:

	void loadTileSet(const TileSetData& tileSetData);
	virtual void culling();
	virtual void updateBorder();
	virtual int posYToRow(float y, float cellHeight, int min, int max, int shift);
	virtual int posXToCol(float y, float cellHeight, int min, int max, int shift);
	virtual bool isRectOnScreen(float posX, float posY, float width, float height);

	bool m_useCulling, m_drawCenter, m_debugCollision, m_borderDirty;
	std::array<Vector2f, 4> m_cullingVertices;
	std::vector<std::pair<int, unsigned int>**> m_layers;
	size_t m_cols, m_rows;
	std::vector<Cell> m_cellsBackground;
	std::vector<Cell> m_cellsMain;
	std::vector<Rect> m_staticRects;
	std::vector<CollisionRect> m_dynamicRects;
	TileSet m_tileSet;
	std::unordered_map<std::string, int> m_animationOffsets;
	unsigned int m_spritesheet;
	std::string m_currentTileset;
	
	float m_left, m_right, m_bottom, m_top, m_screeBorder;
	float m_mapHeight, m_tileHeight, m_tileWidth;

	std::vector<Cell> m_visibleCellsBackground;
	std::vector<Cell> m_visibleCellsMain;

	const Camera& camera;

	static std::unordered_map<std::string, TileSetData> TileSets;
};