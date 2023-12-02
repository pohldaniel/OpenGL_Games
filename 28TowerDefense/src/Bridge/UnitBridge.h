#pragma once
#include <memory>
#include <vector>
#include <engine/Vector.h>
#include <engine/Rect.h>

#include "Beam.h"

class UnitBridge {

public:

	UnitBridge(const Vector2f& setPos);

	void update(float dt, std::vector<std::shared_ptr<Beam>>& listBeams);
	void drawBatched();


	void reset();
	bool checkIfTouchingBeam(std::shared_ptr<Beam> beam);
	void addForceAtPin(Vector2f& forceSum);
	bool getIsStuck();
	bool getReachedEndOfLevel();

	static void Init(const TextureRect& rect);

private:

	bool checkOverlapWithBridgeAndLand(const Vector2f& posCheck, std::vector<std::shared_ptr<Beam>>& listBeams, bool isFirstIteration);


	const Vector2f posStart;
	Vector2f pos;
	float speedY = 0.0f;
	static const float speedX, size, mass;

	bool isStuck = false, reachedEndOfLevel = false;
	std::vector<std::weak_ptr<Beam>> listBeamsTouching;
	static TextureRect Rect;
};
