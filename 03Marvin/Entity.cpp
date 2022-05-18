#include "Entity.h"

Entity::Entity(unsigned int category, const float& dt, const float& fdt) : RenderableObject(category) , m_dt(dt), m_fdt(fdt) {
	/*m_size = Vector2f(26.0f, 26.0f);
	m_position = Vector2f(200.0f, 600.0f);

	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	m_quad = new Quad(true, 0.0f, 1.0f, 0.0f, 1.0f, m_size[0], m_size[1], 0.0f, 0.0f, 1.0f, 1.0f, 0, 0);
	m_quad->setFlipped(true);
	initAnimations();*/
}

Entity::~Entity() {
}


