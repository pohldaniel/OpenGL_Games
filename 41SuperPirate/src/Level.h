#pragma once

#include <memory>
#include <engine/Camera.h>
#include <engine/TileSet.h>

#include "DataStructs.h"

class SpriteEntity;
class Player;

class Level {

public:

	Level(const Camera& camera);
	~Level();
	
	void draw();
	void update(float dt);
	void resize();
	void loadZone(const std::string path, const std::string currentTileset);	
	void loadTileSetData(const std::string& path);

	void setUseCulling(bool useCulling);
	void setDebugCollision(bool debugCollision);
	
	const std::vector<Rect>& getCollisionRects();
	const std::vector<std::unique_ptr<SpriteEntity>>& getSpriteEntities();
	float getMapHeight();
	Player& getPlayer();

private:

	void loadTileSet(const TileSetData& tileSetData);
	void culling();
	void updateBorder();
	int posYToRow(float y, float cellHeight, int min, int max, int shift);
	int posXToCol(float y, float cellHeight, int min, int max, int shift);
	bool isRectOnScreen(float posX, float posY, float width, float height);

	bool m_useCulling, m_debugCollision, m_borderDirty;
	float m_screeBorder;
	std::array<Vector2f, 4> m_cullingVertices;
	std::vector<std::pair<int, unsigned int>**> m_layers;
	size_t m_playerIndex, m_cols, m_rows;
	std::vector<Cell> m_cellsBackground;
	std::vector<Cell> m_cellsMain;
	std::vector<Rect> m_collisionRects;
	TileSet m_tileSet;
	std::unordered_map<std::string, unsigned int> m_charachterOffsets;
	unsigned int m_spritesheet;
	std::string m_currentTileset;

	std::vector<Cell> m_visibleCellsBackground;
	std::vector<Cell> m_visibleCellsMain;
	std::vector<std::unique_ptr<SpriteEntity>> m_spriteEntities;

	float m_left, m_right, m_bottom, m_top;
	float m_mapHeight, m_tileHeight, m_tileWidth;



	const Camera& camera;

	static std::unordered_map<std::string, TileSetData> TileSets;
};