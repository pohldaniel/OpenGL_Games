#pragma once
#include "engine/Vector.h"

#include "RenderableObject.h"

class Entity : public RenderableObject {
public:

	Entity(unsigned int category, const float& dt, const float& fdt);
	virtual ~Entity();

	virtual void fixedUpdate() = 0;
	virtual void update() = 0;
	
private:
	
	const float& m_fdt;
	const float& m_dt;
};