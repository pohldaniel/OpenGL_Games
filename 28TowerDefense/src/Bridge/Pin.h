#pragma once
#include <vector>
#include <memory>
#include <engine/Vector.h>
#include <engine/Rect.h>

class Beam;
class UnitBridge;

class Pin {
public:
	Pin(const Vector2f& setPos, bool setPositionFixed = false);
	void update(float dt);
	void drawBatched();
	void calculateForces(std::vector<std::shared_ptr<Beam>>& listBeams, UnitBridge& unitRedCreature);
	const Vector2f& getPos();

	static void Init(const TextureRect& rect);

private:

	Vector2f pos, vel;
	bool positionFixed;

	Vector2f forceSumStored;
	float massSumStored = 0.0f;

	static TextureRect Rect;
};