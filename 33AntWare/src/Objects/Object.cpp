#include "Object.h"

Matrix4f Object::Transformation;

Object::Object() {
	m_scale.set(1.0f, 1.0f, 1.0f);
	m_position.set(0.0f, 0.0f, 0.0f);
	m_orientation.set(0.0f, 0.0f, 0.0f, 1.0f);
	//m_rotation.set(0.0f, 0.0f, 0.0f);
	m_isDirty = true;
}

void Object::setScale(const float sx, const float sy, const float sz) {
	m_scale.set(sx, sy, sz);
	m_isDirty = true;
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

void Object::setScale(const Vector3f &scale) {
	m_scale = scale;
	m_isDirty = true;
}

void Object::setScale(const float s) {
	setScale(s, s, s);
	m_isDirty = true;
}

void Object::setPosition(const float x, const float y, const float z) {
	m_position.set(x, y, z);
	m_isDirty = true;
}

void Object::setPosition(const Vector3f &position) {
	m_position = position;
	m_isDirty = true;
}

void Object::setOrientation(const Vector3f &axis, float degrees) {
	m_orientation.set(axis, degrees);
	//m_rotation[0] = axis[0] * degrees; m_rotation[0] = axis[1] * degrees; m_rotation[0] = axis[2] * degrees;
	m_isDirty = true;
}

void Object::setOrientation(const float pitch, const float yaw, const float roll) {
	m_orientation.fromPitchYawRoll(pitch, yaw, roll);
	//m_rotation[0] = pitch; m_rotation[0] = yaw; m_rotation[0] = roll;
	m_isDirty = true;
}

void Object::setOrientation(const Vector3f &eulerAngle) {
	m_orientation.fromPitchYawRoll(eulerAngle[0], eulerAngle[1], eulerAngle[2]);
	//m_rotation = eulerAngle;
	m_isDirty = true;
}

void Object::setOrientation(const Quaternion &orientation) {
	m_orientation = orientation;
	m_isDirty = true;
}

void Object::translate(const Vector3f &trans) {
	m_position.translate(trans);
	m_isDirty = true;
}

void Object::translate(const float dx, const float dy, const float dz) {
	m_position.translate(dx, dy, dz);
	m_isDirty = true;
}

void Object::scale(const Vector3f &scale) {
	m_scale.scale(scale);
	m_isDirty = true;
}

void Object::scale(const float sx, const float sy, const float sz) {
	m_scale.scale(sx, sy, sz);
	m_isDirty = true;
}

void Object::scale(const float s) {
	m_scale.scale(s, s, s);
	m_isDirty = true;
}

void Object::rotate(const float pitch, const float yaw, const float roll) {
	m_orientation.rotate(pitch, yaw, roll);
	//m_rotation[0] += pitch; m_rotation[0] += yaw; m_rotation[0] += roll;
	m_isDirty = true;
}

void Object::rotate(const Vector3f& eulerAngle) {
	m_orientation.rotate(eulerAngle[0], eulerAngle[1], eulerAngle[2]);
	//m_rotation += eulerAngle;
	m_isDirty = true;
}

void Object::rotate(const Vector3f &axis, float degrees) {
	m_orientation.rotate(axis, degrees);
	//m_rotation[0] += axis[0] * degrees; m_rotation[0] += axis[1] * degrees; m_rotation[0] += axis[2] * degrees;
	m_isDirty = true;
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

const Matrix4f &Object::getTransformationSOP() {
	Transformation.translate(m_position);
	Transformation *= Matrix4f::Rotate(m_orientation);
	//Transformation *= Matrix4f::Rotate(m_rotation);
	Transformation *= Matrix4f::Scale(m_scale);
	return Transformation;
}

const Matrix4f &Object::getTransformationSO() {
	Transformation.rotate(m_orientation);
	//Transformation *= Matrix4f::Rotate(m_rotation);
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

const Matrix4f &Object::getTransformationOP() {
	Transformation.translate(m_position);
	Transformation *= Matrix4f::Rotate(m_orientation);
	//Transformation *= Matrix4f::Rotate(m_rotation);
	return Transformation;
}

const Matrix4f &Object::getTransformationO() {
	Transformation.rotate(m_orientation);
	//Transformation *= Matrix4f::Rotate(m_rotation);
	return Transformation;
}

const Matrix4f &Object::getTransformationS() {
	Transformation = Matrix4f::Scale(m_scale);
	return Transformation;
}

const Matrix4f &Object::GetTransformation() {
	return Transformation;
}

const bool Object::isDirty() const {
	return m_isDirty;
}

/*Vector3f Object::getRotationAxis(){
	if (m_orientation[3] == 0.0f)
		return Vector3f(1.0f, 0.0f, 0.0f);
	float sqrtOneMinWSquered = sqrt(1 - pow(m_orientation[3], 2));

	return Vector3f(m_orientation[0] / sqrtOneMinWSquered, m_orientation[1] / sqrtOneMinWSquered, m_orientation[2] / sqrtOneMinWSquered);
}*/