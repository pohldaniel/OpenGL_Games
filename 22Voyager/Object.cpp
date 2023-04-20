#include "Object.h"

Object::Object() {
	m_scale.set(1.0f, 1.0f, 1.0f);
	m_position.set(0.0f, 0.0f, 0.0f);
	m_orientation.set(0.0f, 0.0f, 0.0f, 1.0f);
}

void Object::setScale(const float x, const float y, const float z) {
	m_scale.set(x, y, z);
}

void Object::setScale(const Vector3f &scale) {
	m_scale = scale;
}

void Object::setPosition(const float x, const float y, const float z) {
	m_position.set(x, y, z);
}

void Object::setPosition(const Vector3f &position) {
	m_position = position;
}

void Object::setOrientation(const Vector3f &axis, float degrees) {
	m_orientation.set(axis, degrees);
}

void Object::setOrientation(const float degreesX, const float degreesY, const float degreesZ) {
	m_orientation.fromPitchYawRoll(degreesX, degreesY, degreesZ);
}

const Vector3f &Object::getPosition() {
	return m_position;
}

const Vector3f &Object::getScale() {
	return m_scale;
}

const Quaternion &Object::getOrientation() {
	return m_orientation;
}