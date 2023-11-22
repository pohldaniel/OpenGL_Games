#pragma once

#include "Entity.h"

class Bee : public Entity {
public:

	Bee(unsigned int category, const float& dt, const float& fdt);
	virtual ~Bee();

	void fixedUpdate() override {};
	void update() override;
	void render() override;

private:
	void initAnimations() override;
};