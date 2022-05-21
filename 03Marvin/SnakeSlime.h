#pragma once

#include "Entity.h"

class SnakeSlime : public Entity {
public:

	SnakeSlime(unsigned int category, const float& dt, const float& fdt);
	virtual ~SnakeSlime();

	void fixedUpdate() override {};
	void update() override;
	void render() override;

private:
	void initAnimations() override;
};