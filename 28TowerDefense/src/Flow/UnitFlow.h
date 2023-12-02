#pragma once

#include <vector>
#include <engine/Vector.h>
#include <engine/Texture.h>
#include "LevelFlow.h"

class UnitFlow {
public:

	UnitFlow(const Vector2f& setPos);
	

	void update(float dT, LevelFlow& level, std::vector<UnitFlow>& listUnits);
	void drawBatched(float tileSize);
	bool checkOverlap(const Vector2f& posOther, float sizeOther);
	Vector2f computeNormalSeparation(std::vector<UnitFlow>& listUnits);

	static void Inti(const TextureRect& rect);

private:

	Vector2f pos, posDraw;
	static const float speed;
	static const float size;
	static TextureRect Rect;
};