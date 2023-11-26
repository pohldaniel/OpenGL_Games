#pragma once

#include <memory>
#include <vector>
#include <engine/Vector.h>
#include <engine/Rect.h>
#include "MathAddon.h"
#include "Unit.h"
#include "Projectile.h"
#include "Timer.h"

class Turret {

public:

	Turret(const Vector2f&);
	void update(float dt, std::vector<std::shared_ptr<Unit>>& listUnits, std::vector<Projectile>& listProjectiles);
	void drawBatched(float tileSize);
	bool checkIfOnTile(int x, int y);

	static void Init(std::vector<TextureRect>& textureRects);

private:

	void drawTextureWithOffset(const TextureRect& rect, float tileSize, float offset);
	std::weak_ptr<Unit> findEnemyUnit(std::vector<std::shared_ptr<Unit>>& listUnits);
	bool updateAngle(float dt);
	void shootProjectile(std::vector<Projectile>& listProjectiles);

	Vector2f pos;
	float angle;
	static const float speedAngular, weaponRange;
	std::weak_ptr<Unit> unitTarget;
	Timer timerWeapon;

	static TextureRect Rect;
	static TextureRect ShadowRect;
};