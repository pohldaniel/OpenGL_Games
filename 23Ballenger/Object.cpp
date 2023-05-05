#include "Object.h"

Matrix4f Object::Transformation;

Object::Object() {
	m_scale.set(1.0f, 1.0f, 1.0f);
	m_position.set(0.0f, 0.0f, 0.0f);
	m_orientation.set(0.0f, 0.0f, 0.0f, 1.0f);
}

void Object::setScale(const float sx, const float sy, const float sz) {
	m_scale.set(sx, sy, sz);
}

void Object::setScale(const Vector3f &scale) {
	m_scale = scale;
}

void Object::setScale(const float s) {
	setScale(s, s, s);
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

void Object::setOrientation(const Vector3f &eulerAngle) {
	m_orientation.fromPitchYawRoll(eulerAngle[0], eulerAngle[1], eulerAngle[2]);
}

void Object::translate(const Vector3f &trans) {
	m_position.translate(trans);
}

void Object::translate(const float dx, const float dy, const float dz) {
	m_position.translate(dx, dy, dz);
}

void Object::scale(const Vector3f &scale) {
	m_scale.scale(scale);
}

void Object::scale(const float sx, const float sy, const float sz) {
	m_scale.scale(sx, sy, sz);
}

void Object::scale(const float s) {
	m_scale.scale(s, s, s);
}

void Object::rotate(const float pitch, const float yaw, const float roll) {
	m_orientation.rotate(pitch, yaw, roll);
}

void Object::rotate(const Vector3f &axis, float degrees) {
	m_orientation.rotate(axis, degrees);
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

const Matrix4f &Object::getTransformationSOP() {
	Transformation.translate(m_position);
	Transformation *= Matrix4f::Rotate(m_orientation);
	Transformation *= Matrix4f::Scale(m_scale);
	return Transformation;
}

const Matrix4f &Object::getTransformationSO() {
	Transformation.rotate(m_orientation);
	Transformation *= Matrix4f::Scale(m_scale);
	return Transformation;
}

const Matrix4f &Object::getTransformationSP() {
	Transformation.translate(m_position);
	Transformation *= Matrix4f::Scale(m_scale);
	return Transformation;
}

const Matrix4f &Object::getTransformationP() {
	Transformation.translate(m_position);
	return Transformation;
}