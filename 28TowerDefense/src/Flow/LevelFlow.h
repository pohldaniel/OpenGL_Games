#pragma once

#include <queue>
#include <engine/Rect.h>
#include <engine/Camera.h>

struct TileFlow {
	bool isWall;
	int flowDirectionX;
	int flowDirectionY;
	unsigned char flowDistance;
};

class LevelFlow {

public:

	static const unsigned char flowDistanceMax = 255;

	LevelFlow(const Camera& camera, int setTileCountX, int setTileCountY);
	~LevelFlow();

	void draw(float size);

	void init(std::vector<TextureRect>& textureRects);
	bool isTileWall(int x, int y);
	void setTileWall(int x, int y, bool setWall);
	Vector2f getTargetPos();
	void setTargetAndCalculateFlowField(int targetXNew, int targetYNew);
	void calculateDistances();
	Vector2f getFlowNormal(int x, int y);
	void calculateFlowDirections();

private:

	const Camera& camera;
	std::vector<TileFlow> m_tiles;
	std::vector<TextureRect> m_rextureRects;
	const int tileCountX, tileCountY;
	int targetX = -1, targetY = -1;
};