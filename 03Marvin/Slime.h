#pragma once

#include "Entity.h"

class Slime : public Entity {
public:

	Slime(unsigned int category, const float& dt, const float& fdt);
	virtual ~Slime();

	void fixedUpdate() override {};
	void update() override;
	void render() override;

private:
	void initAnimations() override;
};