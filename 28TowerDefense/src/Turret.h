#pragma once

#include <vector>
#include <engine/Vector.h>
#include <engine/Rect.h>
#include "MathAddon.h"

class Turret {

public:
	Turret(const Vector2f&);
	

	void update(float dt);
	void drawBatched(float tileSize);
	bool checkIfOnTile(int x, int y);

	static void Init(std::vector<TextureRect>& textureRects);

private:

	void drawTextureWithOffset(const TextureRect& rect, float tileSize, float offset);

	Vector2f pos;
	float angle;
	static const float speedAngular;

	static TextureRect Rect;
	static TextureRect ShadowRect;
};