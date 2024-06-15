#pragma once

#include <Entities/SpriteEntity.h>

class Character : public SpriteEntity {

public:

	Character();
	virtual ~Character();

	void update(float dt) override;
};