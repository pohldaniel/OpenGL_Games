#pragma once
#include <memory>
#include <vector>
#include <engine/Vector.h>
#include <engine/Rect.h>
#include "Unit.h"

class Projectile{

public:

	Projectile(const Vector2f& setPos, const Vector2f& setDirectionNormal);
	void update(float dt, std::vector<std::shared_ptr<Unit>>& listUnits);
	void drawBatched(float tileSize);
	bool getCollisionOccurred();

	static void Init(const TextureRect& textureRect);

private:
	void checkCollisions(std::vector<std::shared_ptr<Unit>>& listUnits);

	Vector2f pos, directionNormal;
	static const float speed, size, distanceTraveledMax;
	float distanceTraveled = 0.0f;
	bool collisionOccurred = false;

	static TextureRect Rect;
};