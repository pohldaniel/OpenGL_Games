#include "Object.h"

thread_local Matrix4f Object2D::Transformation;

Object2D::Object2D() {
	m_position.set(0.0f, 0.0f);
	m_origin.set(0.0f, 0.0f);
	m_scale.set(1.0f, 1.0f);
	m_orientation = 0.0f;
}

Object2D::Object2D(Object2D const& rhs) {
	m_position = rhs.m_position;
	m_origin = rhs.m_origin;
	m_scale = rhs.m_scale;
	m_orientation = rhs.m_orientation;
}

Object2D& Object2D::operator=(const Object2D& rhs) {
	m_position = rhs.m_position;
	m_origin = rhs.m_origin;
	m_scale = rhs.m_scale;
	m_orientation = rhs.m_orientation;
	return *this;
}

Object2D::Object2D(Object2D&& rhs) noexcept : Object2D(rhs) {
	m_position = std::move(rhs.m_position);
	m_origin = std::move(rhs.m_origin);
	m_scale = std::move(rhs.m_scale);
	m_orientation = std::move(rhs.m_orientation);
}

Object2D& Object2D::operator=(Object2D&& rhs) noexcept {
	m_position = std::move(rhs.m_position);
	m_origin = std::move(rhs.m_origin);
	m_scale = std::move(rhs.m_scale);
	m_orientation = std::move(rhs.m_orientation);
	return *this;
}

Object2D::~Object2D() {

}

void Object2D::setScale(float sx, float sy) {
	m_scale.set(sx, sy);
}

void Object2D::setScale(const Vector2f &scale) {
	m_scale = scale;
}

void Object2D::setScale(float s) {
	setScale(s, s);
}

void Object2D::setPosition(float x, float y) {
	m_position.set(x, y);
}

void Object2D::setPosition(const Vector2f &position) {
	m_position = position;
}

void Object2D::setOrigin(float x, float y) {
	m_origin.set(x, y);
}

void Object2D::setOrigin(const Vector2f& origin) {
	m_origin = origin;
}

void Object2D::setOrientation(float degrees) {
	m_orientation = degrees;
}

void Object2D::translate(const Vector2f &trans) {
	m_position.translate(trans);
}

void Object2D::translate(float dx, float dy) {
	m_position.translate(dx, dy);
}

void Object2D::rotate(float degrees) {
	m_orientation += degrees;
}

void Object2D::translateRelative(float _dx, float _dy) {

	float angle = m_orientation * HALF_PI_ON_180;
	float cos = cosf(angle);
	float sin = sinf(angle);

	float dx = _dx * cos - _dy * sin;
	float dy = _dx * sin + _dy * cos;

	m_position[0] += dx;
	m_position[1] += dy;
}

void Object2D::translateRelative(const Vector2f& trans) {
	translateRelative(trans[0], trans[1]);
}

void Object2D::scale(const Vector2f &scale) {
	m_scale.scale(scale);
}

void Object2D::scale(float sx, float sy) {
	m_scale.scale(sx, sy);
}

void Object2D::scale(float s) {
	m_scale.scale(s, s);
}

const Vector2f& Object2D::getPosition() const {
	return m_position;
}

const Vector2f& Object2D::getScale() const {
	return m_scale;
}

float Object2D::getOrientation() const {
	return m_orientation;
}

const Matrix4f& Object2D::getTransformationSOP() const {
	Transformation.translate(m_position);
	Transformation *= Matrix4f::Rotate(m_orientation, m_origin);
	Transformation *= Matrix4f::Scale(m_scale);
	return Transformation;
}

const Matrix4f& Object2D::getTransformationSO() const {
	Transformation.rotate(m_orientation, m_origin);
	Transformation *= Matrix4f::Scale(m_scale);
	return Transformation;
}

const Matrix4f& Object2D::getTransformationSP() const {
	Transformation.translate(m_position);
	Transformation *= Matrix4f::Scale(m_scale);
	return Transformation;
}

const Matrix4f& Object2D::getTransformationOP()  const {
	Transformation.translate(m_position);
	Transformation *= Matrix4f::Rotate(m_orientation, m_origin);
	return Transformation;
}

const Matrix4f& Object2D::getTransformationP() const {
	Transformation.translate(m_position);
	return Transformation;
}

const Matrix4f& Object2D::getTransformationO()  const {
	Transformation.rotate(m_orientation, m_origin);
	return Transformation;
}

const Matrix4f& Object2D::getTransformationS() const {
	Transformation.scale(m_scale);
	return Transformation;
}

const Matrix4f& Object2D::GetTransformation() {
	return Transformation;
}

////////////////////////////////////////////////////////////////////////////////////
thread_local Matrix4f Object::Transformation;

Object::Object() {
	m_position.set(0.0f, 0.0f, 0.0f);
	m_origin.set(0.0f, 0.0f, 0.0f);
	m_scale.set(1.0f, 1.0f, 1.0f);	
	m_orientation.set(0.0f, 0.0f, 0.0f, 1.0f);
}

Object::Object(Object const& rhs) {
	m_position = rhs.m_position;
	m_origin = rhs.m_origin;
	m_scale = rhs.m_scale;
	m_orientation = rhs.m_orientation;	
}

Object& Object::operator=(const Object& rhs) {
	m_position = rhs.m_position;
	m_origin = rhs.m_origin;
	m_scale = rhs.m_scale;
	m_orientation = rhs.m_orientation;
	return *this;
}

Object::Object(Object&& rhs) noexcept : Object(rhs) {
	m_position = std::move(rhs.m_position);
	m_origin = std::move(rhs.m_origin);
	m_scale = std::move(rhs.m_scale);
	m_orientation = std::move(rhs.m_orientation);
}

Object& Object::operator=(Object&& rhs) noexcept {
	m_position = std::move(rhs.m_position);
	m_origin = std::move(rhs.m_origin);
	m_scale = std::move(rhs.m_scale);
	m_orientation = std::move(rhs.m_orientation);
	return *this;
}

void Object::setScale(float sx, float sy, float sz) {
	m_scale.set(sx, sy, sz);
}

void Object::setScale(const Vector3f &scale) {
	m_scale = scale;
}

void Object::setScale(float s) {
	setScale(s, s, s);
}

void Object::setPosition(float x, float y, float z) {
	m_position.set(x, y, z);
}

void Object::setPosition(const Vector3f &position) {
	m_position = position;
}

void Object::setOrigin(float x, float y, float z) {
	m_origin.set(x, y, z);
}

void Object::setOrigin(const Vector3f& origin) {
	m_origin = origin;
}

void Object::setOrientation(const Vector3f &axis, float degrees) {
	m_orientation.set(axis, degrees);
}

void Object::setOrientation(float pitch, float yaw, float roll) {
	m_orientation.fromPitchYawRoll(pitch, yaw, roll);
}

void Object::setOrientation(const Vector3f &eulerAngle) {
	m_orientation.fromPitchYawRoll(eulerAngle[0], eulerAngle[1], eulerAngle[2]);
}

void Object::setOrientation(const Quaternion &orientation) {
	m_orientation = orientation;
}

void Object::setOrientation(float x, float y, float z, float w) {
	m_orientation.set(x, y, z, w);
}

void Object::translate(const Vector3f &trans) {
	m_position.translate(trans);
}

void Object::translate(float dx, float dy, float dz) {
	m_position.translate(dx, dy, dz);
}

void Object::translateRelative(const Vector3f& trans) {
	m_position += Quaternion::Rotate(m_orientation, trans);
}

void Object::translateRelative(float dx, float dy, float dz) {
	m_position += Quaternion::Rotate(m_orientation, Vector3f(dx, dy, dz));
}

void Object::scale(const Vector3f &scale) {
	m_scale.scale(scale);
}

void Object::scale(float sx, float sy, float sz) {
	m_scale.scale(sx, sy, sz);
}

void Object::scale(float s) {
	m_scale.scale(s, s, s);
}

void Object::rotate(float pitch, float yaw, float roll) {
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

void Object::rotate(float x, float y, float z, float w) {
	m_orientation.rotate(x, y, z, w);
}

const Vector3f& Object::getPosition() const {
	return m_position;
}

const Vector3f& Object::getScale() const {
	return m_scale;
}

const Quaternion& Object::getOrientation() const {
	return m_orientation;
}

Vector3f& Object::getPosition() {
	return m_position;
}

Vector3f& Object::getScale() {
	return m_scale;
}

Quaternion& Object::getOrientation() {
	return m_orientation;
}

const Matrix4f& Object::getTransformationSOP() const{
	Transformation.translate(m_position);
	Transformation *= Matrix4f::Rotate(m_orientation, m_origin);
	Transformation *= Matrix4f::Scale(m_scale);
	return Transformation;
}

const Matrix4f& Object::getTransformationSO() const{
	Transformation.rotate(m_orientation, m_origin);
	Transformation *= Matrix4f::Scale(m_scale);
	return Transformation;
}

const Matrix4f& Object::getTransformationSP() const{
	Transformation.translate(m_position);
	Transformation *= Matrix4f::Scale(m_scale);
	return Transformation;
}

const Matrix4f& Object::getTransformationOP()  const {
	Transformation.translate(m_position);
	Transformation *= Matrix4f::Rotate(m_orientation, m_origin);
	return Transformation;
}

const Matrix4f& Object::getTransformationP() const{
	Transformation.translate(m_position);
	return Transformation;
}

const Matrix4f& Object::getTransformationO()  const{
	Transformation.rotate(m_orientation, m_origin);
	return Transformation;
}

const Matrix4f& Object::getTransformationS()  const{
	Transformation.scale(m_scale);
	return Transformation;
}

const Matrix4f& Object::GetTransformation() {
	return Transformation;
}