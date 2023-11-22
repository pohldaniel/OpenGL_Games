#pragma once

#include "Entity.h"

class GrassBlock : public Entity {
public:

	GrassBlock(unsigned int category, const float& dt, const float& fdt);
	virtual ~GrassBlock();

	void fixedUpdate() override {};
	void update() override;
	void render() override;

private:
	void initAnimations() override;
};