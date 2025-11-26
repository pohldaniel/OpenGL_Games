#pragma once

#include <string>
#include <unordered_map>
#include <engine/Camera.h>
#include <engine/Rect.h>
#include <engine/TileSet.h>
#include <engine/Framebuffer.h>

#include "DataStructs.h"
#include "Zone.h"

class Overworld : public Zone {

	static const int MAX_POINTS = 1000;

public:

	Overworld(const Camera& camera, const bool initDebug = true);
	~Overworld();

	void draw() override;
	void update(float dt) override;
	void resize() override;
	void loadTileSetData(const std::string& path) override;
	void loadZone(const std::string path, const std::string currentTileset) override;
	
	void setDrawCenter(bool drawCenter) override;
	void setUseCulling(bool useCulling) override;
	void setDebugCollision(bool debugCollision)override;

private:

	void loadTileSet(const TileSetData& tileSetData) override;
	void culling() override;
	void updateBorder() override;
	int posYToRow(float y, float cellHeight, int min, int max, int shift) override;
	int posXToCol(float y, float cellHeight, int min, int max, int shift) override;
	bool isRectOnScreen(float posX, float posY, float width, float height) override;
	void updatePoints() override;
	void initDebug() override;

	bool m_drawCenter, m_useCulling,  m_debugCollision, m_borderDirty;
	std::array<Vector2f, 4> m_cullingVertices;
	std::vector<std::pair<int, unsigned int>**> m_layers;
	size_t m_cols, m_rows;
	std::vector<Cell> m_cellsBackground;
	std::vector<Cell> m_cellsMain;
	std::vector<Rect> m_collisionRects;
	TileSet m_tileSet;
	std::unordered_map<std::string, unsigned int> m_charachterOffsets;
	unsigned int m_spritesheet;
	std::string m_currentTileset;

	float m_left, m_right, m_bottom, m_top, m_screeBorder;
	float m_mapHeight, m_tileHeight, m_tileWidth;

	std::vector<Cell> m_visibleCellsBackground;
	std::vector<Cell> m_visibleCellsMain;

	unsigned int m_vao, m_vbo;
	Vector3f* m_pointBatch, * m_pointBatchPtr;
	uint32_t m_pointCount;

	Framebuffer m_mainRenderTarget;

	const Camera& camera;

	static std::unordered_map<std::string, TileSetData> TileSets;
};