#pragma once

class Entity {

public:

	Entity() = default;
	virtual ~Entity() = default;
	virtual void update(const float dt) = 0;

};