#include "Object.h"

Object::Object(unsigned int category) {
	m_category = category;
}

void Object::setCategory(unsigned int category) {
	m_category = category;
}

const unsigned int Object::getCategory() const {
	return m_category;
}

void Object::setSize(const float x, const float y) {
	m_size[0] = x;
	m_size[1] = y;
}

void Object::setSize(const Vector2f &size) {
	m_size = size;
}

void Object::setPosition(const float x, const float y) {
	m_position[0] = x;
	m_position[1] = y;
	m_transform.translate((m_position[0] - m_origin[0]), (m_position[1] - m_origin[1]), 0.0f);
}

void Object::setPosition(const Vector2f &position) {
	m_position = position;
	m_transform.translate((m_position[0] - m_origin[0]), (m_position[1] - m_origin[1]), 0.0f);
}

void Object::setOrigin(const float x, const float y) {
	m_origin[0] = x;
	m_origin[1] = y;
	m_transform.translate((m_position[0] - m_origin[0]), (m_position[1] - m_origin[1]), 0.0f);
}

void Object::setOrigin(const Vector2f &origin) {
	m_origin = origin;
	m_transform.translate((m_position[0] - m_origin[0]), (m_position[1] - m_origin[1]), 0.0f);
}

const Vector2f &Object::getPosition() const {
	return m_position;
}

const Vector2f &Object::getSize() const {
	return m_size;
}

const Vector2f &Object::getOrigin() const {
	return m_origin;
}