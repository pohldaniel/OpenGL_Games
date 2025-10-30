#pragma once

#include <engine/Rect.h>
#include <engine/Camera.h>
#include "SpriteEntity.h"

class Player : public SpriteEntity {

public:

	Player(Cell& cell, Camera& camera, const std::vector<Rect>& collisionRects);
	virtual ~Player();

private:

	const std::vector<Rect>& collisionRects;
	Camera& camera;
};