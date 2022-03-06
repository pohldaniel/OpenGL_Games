#include "Entity.h"

Entity::Entity(const float& dt, const float& fdt) : i_dt(dt), i_fdt(fdt){}

Entity::~Entity() {
}

Collision Entity::getCollider() { 
	return m_collider;
}

void Entity::setSize(float x, float y) {
	m_size[0] = x;
	m_size[1] = y;

	xScale = x / (float)(WIDTH);
	yScale = y / (float)(HEIGHT);	
}

void Entity::setSize(const Vector2f &size) {
	m_size = size;

	xScale = size[0] / (float)(WIDTH);
	yScale = size[1] / (float)(HEIGHT);
}

void Entity::setPosition(float x, float y) {
	m_position[0] = x;
	m_position[1] = y;

	//m_transform.translate(x  * xTrans - 1.0f, yTrans * (HEIGHT - y) - 1.0f, 0.0f);
	m_transform.translate(x * xTrans - 1.0f, yTrans * (HEIGHT - y + 0.5f * m_size[1]) - 1.0f, 0.0f);
}

void Entity::setPosition(const Vector2f &position) {
	m_position = position;

	//m_transform.translate(position[0]  * xTrans - 1.0f, yTrans * (HEIGHT - position[1]) - 1.0f, 0.0f);
	m_transform.translate(position[0] * xTrans - 1.0f, yTrans * (HEIGHT - position[1] + 0.5f * m_size[1]) - 1.0f, 0.0f);
}

const Vector2f &Entity::getPosition() const{
	return m_position;
}

const Vector2f &Entity::getSize() const {
	return m_size;
}
