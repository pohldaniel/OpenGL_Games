#pragma once

#include <vector>
#include <engine/Vector.h>
#include <engine/Texture.h>
#include "Level.h"

class Unit{
public:

	Unit(const Vector2f& setPos);
	

	void update(float dT, Level& level, std::vector<Unit>& listUnits);
	void drawBatched(int tileSize);
	bool checkOverlap(const Vector2f& posOther, float sizeOther);
	Vector2f computeNormalSeparation(std::vector<Unit>& listUnits);

	static void Inti(const TextureRect& rect);

private:

	Vector2f pos, posDraw;
	static const float speed;
	static const float size;
	static TextureRect Rect;
};