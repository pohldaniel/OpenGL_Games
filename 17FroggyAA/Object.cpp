#include "Object.h"

Object::Object() {}

void Object::setSize(const float x, const float y, const float z) {
	m_size[0] = x;
	m_size[1] = y;
	m_size[2] = y;
}

void Object::setSize(const Vector3f &size) {
	m_size = size;
}

void Object::setPosition(const float x, const float y, const float z) {
	m_position[0] = x;
	m_position[1] = y;
	m_position[2] = z;
	m_transform.translate((m_position[0] - m_origin[0]), (m_position[1] - m_origin[1]), 0.0f);
}

void Object::setPosition(const Vector3f &position) {
	m_position = position;
	m_transform.translate((m_position[0] - m_origin[0]), (m_position[1] - m_origin[1]), 0.0f);
}

void Object::setOrigin(const float x, const float y, const float z) {
	m_origin[0] = x;
	m_origin[1] = y;
	m_origin[2] = z;
	m_transform.translate((m_position[0] - m_origin[0]), (m_position[1] - m_origin[1]), 0.0f);
}

void Object::setOrigin(const Vector3f &origin) {
	m_origin = origin;
	m_transform.translate((m_position[0] - m_origin[0]), (m_position[1] - m_origin[1]), 0.0f);
}

const Vector3f &Object::getPosition() const {
	return m_position;
}

const Vector3f &Object::getSize() const {
	return m_size;
}

const Vector3f &Object::getOrigin() const {
	return m_origin;
}