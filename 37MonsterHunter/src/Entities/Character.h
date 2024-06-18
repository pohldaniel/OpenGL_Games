#pragma once
#include <Entities/SpriteEntity.h>

class Character : public SpriteEntity {

public:

	Character(Cell& cell);
	virtual ~Character();

	void update(float dt) override;
};