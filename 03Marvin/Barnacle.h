#pragma once

#include "Entity.h"

class Barnacle : public Entity {
public:

	Barnacle(unsigned int category, const float& dt, const float& fdt);
	virtual ~Barnacle();

	void fixedUpdate() override {};
	void update() override;
	void render() override;

private:
	void initAnimations() override;	
};