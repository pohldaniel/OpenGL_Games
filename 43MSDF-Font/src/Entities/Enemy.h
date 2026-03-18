#pragma once

#include "SpriteEntity.h"

class Enemy : public SpriteEntity {

public:

	Enemy(Cell& cell, float elpasedTime = 0.0f, int framecount = 4);
	virtual ~Enemy();

	void update(float dt) override;
};