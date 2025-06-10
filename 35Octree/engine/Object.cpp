#include <iostream>
#include "Object.h"

thread_local Matrix4f Object::Transformation;

Object::Object() {
	m_scale.set(1.0f, 1.0f, 1.0f);
	m_position.set(0.0f, 0.0f, 0.0f);
	m_orientation.set(0.0f, 0.0f, 0.0f, 1.0f);
	//m_rotation.set(0.0f, 0.0f, 0.0f);
}

Object::Object(Object const& rhs) {
	m_position = rhs.m_position;
	m_scale = rhs.m_scale;
	m_orientation = rhs.m_orientation;	
	//m_rotation = rhs.m_rotation;
}

Object& Object::operator=(const Object& rhs) {
	m_position = rhs.m_position;
	m_scale = rhs.m_scale;
	m_orientation = rhs.m_orientation;
	//m_rotation = rhs.m_rotation;
	return *this;
}

Object::Object(Object&& rhs) : Object(rhs) {
	m_position = rhs.m_position;
	m_scale = rhs.m_scale;
	m_orientation = rhs.m_orientation;
	//m_rotation = rhs.m_rotation;
}

Object& Object::operator=(Object&& rhs) {
	m_position = rhs.m_position;
	m_scale = rhs.m_scale;
	m_orientation = rhs.m_orientation;
	//m_rotation = rhs.m_rotation;
	return *this;
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

void Object::setOrigin(const float x, const float y, const float z) {
	m_origin.set(x, y, z);
}

void Object::setOrigin(const Vector3f& origin) {
	m_origin = origin;
}

void Object::setOrientation(const Vector3f &axis, float degrees) {
	m_orientation.set(axis, degrees);
}

void Object::setOrientation(const float pitch, const float yaw, const float roll) {
	m_orientation.fromPitchYawRoll(pitch, yaw, roll);
}

void Object::setOrientation(const Vector3f &eulerAngle) {
	m_orientation.fromPitchYawRoll(eulerAngle[0], eulerAngle[1], eulerAngle[2]);
}

void Object::setOrientation(const Quaternion &orientation) {
	m_orientation = orientation;
}

void Object::setOrientation(const float x, const float y, const float z, const float w) {
	m_orientation.set(x, y, z, w);
}

void Object::translate(const Vector3f &trans) {
	m_position.translate(trans);
}

void Object::translate(const float dx, const float dy, const float dz) {
	m_position.translate(dx, dy, dz);
}

void Object::translateRelative(const Vector3f& trans) {
	m_position += Quaternion::Rotate(m_orientation, trans);
}

void Object::translateRelative(const float dx, const float dy, const float dz) {
	m_position += Quaternion::Rotate(m_orientation, Vector3f(dx, dy, dz));
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

void Object::rotate(const Vector3f& eulerAngle) {
	m_orientation.rotate(eulerAngle[0], eulerAngle[1], eulerAngle[2]);
}

void Object::rotate(const Vector3f &axis, float degrees) {
	m_orientation.rotate(axis, degrees);
}

void Object::rotate(const Quaternion& orientation) {
	m_orientation *= orientation;
}

void Object::rotate(const float x, const float y, const float z, const float w) {
	m_orientation.rotate(x, y, z, w);
}

const Vector3f &Object::getPosition() const {
	return m_position;
}

const Vector3f &Object::getScale() const {
	return m_scale;
}

const Quaternion &Object::getOrientation() const {
	return m_orientation;
}

/*const Vector3f& Object::getRotation() const {
	return m_rotation;
}*/

const Matrix4f &Object::getTransformationSOP() const{
	Transformation.translate(m_position);
	Transformation *= Matrix4f::Rotate(m_orientation);
	//Transformation *= Matrix4f::Rotate(m_rotation);
	Transformation *= Matrix4f::Scale(m_scale);
	return Transformation;
}

const Matrix4f &Object::getTransformationSO() const{
	Transformation.rotate(m_orientation);
	//Transformation *= Matrix4f::Rotate(m_rotation);
	Transformation *= Matrix4f::Scale(m_scale);
	return Transformation;
}

const Matrix4f &Object::getTransformationSP() const{
	Transformation.translate(m_position);
	Transformation *= Matrix4f::Scale(m_scale);
	return Transformation;
}

const Matrix4f &Object::getTransformationP() const{
	Transformation.translate(m_position);
	return Transformation;
}

const Matrix4f &Object::getTransformationOP()  const{
	Transformation.translate(m_position);
	Transformation *= Matrix4f::Rotate(m_orientation);
	//Transformation *= Matrix4f::Rotate(m_rotation);
	return Transformation;
}

const Matrix4f &Object::getTransformationO()  const{
	Transformation.rotate(m_orientation);
	//Transformation *= Matrix4f::Rotate(m_rotation);
	return Transformation;
}

const Matrix4f &Object::getTransformationS()  const{
	Transformation = Matrix4f::Scale(m_scale);
	return Transformation;
}

const Matrix4f &Object::GetTransformation() {
	return Transformation;
}
