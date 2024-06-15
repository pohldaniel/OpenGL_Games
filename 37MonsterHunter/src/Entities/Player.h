#pragma once

#include <Entities/SpriteEntity.h>

class Player : public SpriteEntity {

public:

	Player();
	virtual ~Player();

	void update(float dt) override;
};