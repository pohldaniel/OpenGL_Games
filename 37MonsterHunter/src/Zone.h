#pragma once

#include <vector>
#include <array>
#include <random>
#include <engine/Vector.h>
#include <engine/Camera.h>
#include <engine/Rect.h>
#include <engine/TileSet.h>
#include <engine/Framebuffer.h>

#include "MonsterIndex.h"
#include "Fade.h"
#include "Timer.h"

class SpriteEntity;
class Player;
class Character;

struct Biome {
	std::string biome;
	Rect rect;
	std::vector<MonsterEntry> monsters;
};

struct TileSetData {
	std::vector<std::pair<std::string, float>> pathSizes;
	std::vector<std::pair<std::string, unsigned int>> offsets;
};

struct Transition {
	std::string target;
	std::string newPos;
	Rect collisionRect;
};

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
	void loadZone(const std::string path, const std::string currentTileset, const std::string position);
	void initDebug();
	void updateBorder();
	

	const std::vector<Rect>& getCollisionRects();
	const std::vector<std::unique_ptr<SpriteEntity>>& getSpriteEntities();
	const std::vector<std::reference_wrapper<Character>>& getCharacters();
	const std::vector<Transition>& getTransitions();
	const std::vector<Biome>& getBiomes();
	float getMapHeight();
	Fade& getFade();
	Player& getPlayer();

	void setUseCulling(bool useCulling);
	void setScreeBorder(float screeBorder);
	void setDrawScreenBorder(bool drawScreenBorder);
	void setDrawCenter(bool drawCenter);
	void setDebugCollision(bool debugCollision);
	void setSpritesheet(const unsigned int& spritesheet);
	void setAlpha(float alpha);
	void loadTileSetData(const std::string& path);
	const int getBiomeIndex() const;

private:

	void updatePoints();
	void culling();
	void drawCullingRect();
	int posYToRow(float y, float cellHeight, int min, int max, int shift);
	int posXToCol(float y, float cellHeight, int min, int max, int shift);
	bool isRectOnScreen(float posX, float posY, float width, float height);
	void loadTileSet(const std::vector<std::pair<std::string, float>>& pathSizes, const std::vector<std::pair<std::string, unsigned int>>& offsets);
	
	std::vector<std::pair<int, unsigned int>**> m_layers;
	std::vector<AnimatedCell> m_cellsAnimated;
	std::vector<Cell> m_cellsBackground;
	std::vector<CellShadow> m_cellsMain;
	std::vector<Transition> m_transitions;

	std::vector<AnimatedCell> m_visibleCellsAnimated;
	std::vector<Cell> m_visibleCellsBackground;
	std::vector<CellShadow> m_visibleCellsMain;

	std::vector<Rect> m_collisionRects;
	//std::vector<Rect> m_monsterRects;

	std::vector<std::unique_ptr<SpriteEntity>> m_spriteEntities;
	std::vector<std::reference_wrapper<Character>> m_characters;
	std::vector<Biome> m_biomes;

	float m_mapHeight, m_tileHeight, m_tileWidth;
	float m_left, m_right, m_bottom, m_top;
	float m_screeBorder;
	bool m_useCulling, m_drawScreenBorder, m_drawCenter, m_debugCollision;
	bool m_borderDirty;
	std::array<Vector2f, 4> m_cullingVertices;
	size_t m_playerIndex, m_cols, m_rows;

	unsigned int m_spritesheet;
	unsigned int m_vao, m_vbo;
	Vector3f *pointBatch, *pointBatchPtr;
	uint32_t m_pointCount;
	Timer m_monsterEncounter;
	const Camera& camera;

	float m_elapsedTime;
	int m_currentFrame;
	int m_frameCount;
	int m_biomeIndex;

	std::string m_currentTileset;
	int m_playerOffset, m_waterOffset, m_coastOffset;

	float m_alpha;
	Fade m_fade;
	Framebuffer m_mainRenderTarget;
	TileSet m_tileSet;
	std::unordered_map<std::string, unsigned int> m_charachterOffsets;

	static int CheckMonsterCollision(const SpriteEntity* player, const std::vector<Biome>& biomes);
	static std::unordered_map<std::string, TileSetData> TileSets;
	static std::random_device RandomDevice;
	static std::mt19937 MersenTwist;
	static std::uniform_int_distribution<int> Distribution;
};