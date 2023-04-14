#include "Object.h"

Object::Object() {
	//m_transform.reset();
	m_scale.set(1.0f, 1.0f, 1.0f);
	m_position.set(0.0f, 0.0f, 0.0f);
}

void Object::scale(const float x, const float y, const float z) {
	//m_transform.scale(x, y, z);
	m_scale.set(x, y, z);
}

void Object::scale(const Vector3f &scale) {
	//m_transform.scale(scale);
	m_scale = scale;
}

void Object::setPosition(const float x, const float y, const float z) {
	//m_transform.setPosition(x, y, z);
	m_position.set(x, y, z);
}

void Object::setPosition(const Vector3f &position) {
	//m_transform.setPosition(m_position[0], m_position[1], 0.0f);
	m_position = position;
}

const Vector3f &Object::getPosition() {
	//m_transform.getPosition(m_position);
	return m_position;
}

const Vector3f &Object::getScale() {
	//m_transform.getScale(m_scale);
	return m_scale;
}