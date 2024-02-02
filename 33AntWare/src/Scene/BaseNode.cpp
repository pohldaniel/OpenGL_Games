#include <iostream>
#include "BaseNode.h"

BaseNode::BaseNode() : IBaseNode(), m_markForRemove(false), m_isDirty(true) {

}

void BaseNode::markForRemove() {
	m_markForRemove = true;
}

void BaseNode::OnTransformChanged() {

	if (m_isDirty) {
		return;
	}
	
	for (auto&& child : m_children) {
		child->OnTransformChanged();
	}

	m_isDirty = true;
}

void BaseNode::setScale(const float sx, const float sy, const float sz) {
	Object::setScale(sx, sy, sz);
	OnTransformChanged();
}

void BaseNode::setScale(const Vector3f& scale) {
	Object::setScale(scale);
	OnTransformChanged();
}

void BaseNode::setScale(const float s) {
	Object::setScale(s);
	OnTransformChanged();
}

void BaseNode::setPosition(const float x, const float y, const float z) {
	Object::setPosition(x, y, z);
	OnTransformChanged();
}

void BaseNode::setPosition(const Vector3f& position) {
	Object::setPosition(position);
	OnTransformChanged();	
}

void BaseNode::setOrientation(const Vector3f& axis, float degrees) {
	Object::setOrientation(axis, degrees);
	OnTransformChanged();
}

void BaseNode::setOrientation(const float degreesX, const float degreesY, const float degreesZ) {
	Object::setOrientation(degreesX, degreesY, degreesZ);
	OnTransformChanged();
}

void BaseNode::setOrientation(const Vector3f& euler) {
	Object::setOrientation(euler);
	OnTransformChanged();
}

void BaseNode::setOrientation(const Quaternion& orientation) {
	Object::setOrientation(orientation);
	OnTransformChanged();
}

void BaseNode::translate(const Vector3f& trans) {
	Object::translate(trans);
	OnTransformChanged();
}

void BaseNode::translate(const float dx, const float dy, const float dz) {
	Object::translate(dx, dy, dz);
	OnTransformChanged();
}

void BaseNode::translateRelative(const Vector3f& trans) {
	Object::translateRelative(trans);
	OnTransformChanged();
}

void BaseNode::translateRelative(const float dx, const float dy, const float dz) {
	Object::translateRelative(dx, dy, dz);
	OnTransformChanged();
}

void BaseNode::scale(const Vector3f& scale) {
	Object::scale(scale);
	OnTransformChanged();
}

void BaseNode::scale(const float sx, const float sy, const float sz) {
	Object::scale(sx, sy, sz);
	OnTransformChanged();
}

void BaseNode::scale(const float s) {
	Object::scale(s);
	OnTransformChanged();
}

void BaseNode::rotate(const float pitch, const float yaw, const float roll) {
	Object::rotate(pitch, yaw, roll);
	OnTransformChanged();
}

void BaseNode::rotate(const Vector3f& eulerAngle) {
	Object::rotate(eulerAngle);
	OnTransformChanged();
}

void BaseNode::rotate(const Vector3f& axis, float degrees) {
	Object::rotate(axis, degrees);
	OnTransformChanged();
}

void BaseNode::rotate(const Quaternion& orientation) {
	Object::rotate(orientation);
	OnTransformChanged();
}

const Matrix4f& BaseNode::getTransformation() const {
	return getTransformationSOP();
}