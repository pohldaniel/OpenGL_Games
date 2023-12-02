#pragma once

#include <queue>
#include <engine/Rect.h>
#include <engine/Camera.h>

enum TileType : char {
	EMPTY,
	WALL,
	ENEMY_SPAWNER
};

struct Tile {
	TileType type;
	int flowDirectionX;
	int flowDirectionY;
	unsigned char flowDistance;
};

class Level {

public:

	static const unsigned char flowDistanceMax = 255;

	Level(const Camera& camera, int setTileCountX, int setTileCountY);
	~Level();

	void draw(float tileSize);

	void init(std::vector<TextureRect>& textureRects);
	bool isTileWall(int x, int y);
	void setTileWall(int x, int y, bool setWall);
	Vector2f getTargetPos();
	Vector2f getRandomEnemySpawnerLocation();
	void calculateDistances();
	Vector2f getFlowNormal(int x, int y);
	void calculateFlowDirections();

private:

	void calculateFlowField();
	void setTileType(int x, int y, TileType tileType);
	TileType getTileType(int x, int y);

	std::vector<Tile> m_tiles;
	std::vector<TextureRect> m_textureRects;
	const int tileCountX, tileCountY;
	const int targetX = 0, targetY = 0;

	const Camera& camera;
};