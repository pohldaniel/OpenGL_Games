#pragma once

#include <Entities/SpriteEntity.h>

class Monster : public SpriteEntity {

public:

	Monster(Cell& cell);
	virtual ~Monster();

	void update(float dt) override;
};