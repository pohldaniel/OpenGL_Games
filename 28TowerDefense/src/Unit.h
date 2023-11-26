#pragma once

#include <engine/Vector.h>
#include <engine/Rect.h>
#include "Level.h"

class Unit{

public:

	Unit(const Vector2f& setPos);
	
	void update(float dt, Level& level, std::vector<Unit>& listUnits);
	void drawBatched(float tileSize);
	bool checkOverlap(const Vector2f& posOther, float sizeOther);
	Vector2f computeNormalSeparation(std::vector<Unit>& listUnits);
	bool getIsAlive();

	static void Init(const TextureRect& rect);

private:

	Vector2f pos;
	bool isAlive = true;
	static const float speed;
	static const float size;
	static TextureRect Rect;
};