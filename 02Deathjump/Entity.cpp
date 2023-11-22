#include "Entity.h"

Entity::Entity(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt){}

Entity::~Entity() {
	delete m_textureAtlas;
	delete m_currentFrame;
}

Collision Entity::getCollider() { 
	return m_collider;
}

void Entity::setSize(const float x, const float y) {
	m_size[0] = x;
	m_size[1] = y;
}

void Entity::setSize(const Vector2f &size) {
	m_size = size;	
}

void Entity::setPosition(const float x, const float y) {
	m_position[0] = x;
	m_position[1] = y;
	m_transform.translate((m_position[0] - m_origin[0]), (HEIGHT - m_position[1] + m_origin[1]), 0.0f);
}

void Entity::setPosition2(const float x, const float y) {
	m_position[0] = x;
	m_position[1] = y;
	m_transform.translate((m_position[0] - m_origin[0]), (m_position[1] + m_origin[1]), 0.0f);
}

void Entity::setPosition(const Vector2f &position) {
	m_position = position;
	m_transform.translate((m_position[0] - m_origin[0]), (HEIGHT - m_position[1] + m_origin[1]), 0.0f);
}

void Entity::setPosition2(const Vector2f &position) {
	m_position = position;
	m_transform.translate((m_position[0] - m_origin[0]), (m_position[1] + m_origin[1]), 0.0f);
}

void Entity::setOrigin(const float x, const float y) {
	m_origin[0] = x;
	m_origin[1] = y;
	m_transform.translate((m_position[0] - m_origin[0]), (HEIGHT - m_position[1] + m_origin[1]), 0.0f);
}

void Entity::setOrigin2(const float x, const float y) {
	m_origin[0] = x;
	m_origin[1] = y;
	m_transform.translate((m_position[0] - m_origin[0]), (m_position[1] + m_origin[1]), 0.0f);
}

void Entity::setOrigin(const Vector2f &origin) {
	m_origin = origin;
	m_transform.translate((m_position[0] - m_origin[0]), (HEIGHT - m_position[1] + m_origin[1]), 0.0f);
}

void Entity::setOrigin2(const Vector2f &origin) {
	m_origin = origin;
	m_transform.translate((m_position[0] - m_origin[0]), (m_position[1] + m_origin[1]), 0.0f);
}


const Vector2f &Entity::getPosition() const{
	return m_position;
}

const Vector2f &Entity::getSize() const {
	return m_size;
}

const Vector2f &Entity::getOrigin() const {
	return m_origin;
}
