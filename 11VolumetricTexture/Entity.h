#pragma once

#include "RenderableObject.h"

class Entity : public RenderableObject {

public:

	Entity(const float& dt, const float& fdt);
	virtual ~Entity();

	virtual void fixedUpdate() = 0;
	virtual void update() = 0;

protected:

	const float& m_fdt;
	const float& m_dt;

private:
	virtual void initAnimations() = 0;
};