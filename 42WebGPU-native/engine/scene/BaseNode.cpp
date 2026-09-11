#include <iostream>
#include "BaseNode.h"
#include "SceneNode.h"

BaseNode::BaseNode() : Object(), Node(), m_isDirty(true){

}

BaseNode::BaseNode(const BaseNode& rhs) : Node(rhs)  , Object(rhs){
	m_isDirty = rhs.m_isDirty;
}

BaseNode& BaseNode::operator=(const BaseNode& rhs) {
	Node::operator=(rhs);
	Object::operator=(rhs);
	m_isDirty = rhs.m_isDirty;
	return *this;
}

BaseNode::BaseNode(BaseNode&& rhs) : Node(rhs), Object(rhs) {
	m_isDirty = rhs.m_isDirty;
}

BaseNode& BaseNode::operator=(BaseNode&& rhs) {
	Node::operator=(rhs);
	Object::operator=(rhs);
	m_isDirty = rhs.m_isDirty;
	return *this;
}

void BaseNode::OnTransformChanged() const {

	if (m_isDirty) {
		return;
	}
	
	for (auto&& child : m_children) {
		static_cast<BaseNode*>(child.get())->OnTransformChanged();
	}

	m_isDirty = true;
}

void BaseNode::setScale(const float sx, const float sy, const float sz) const {
	Object::setScale(sx, sy, sz);
	OnTransformChanged();
}

void BaseNode::setScale(const Vector3f& scale) const {
	Object::setScale(scale);
	OnTransformChanged();
}

void BaseNode::setScale(const float s) const {
	Object::setScale(s);
	OnTransformChanged();
}

void BaseNode::setPosition(const float x, const float y, const float z) const {
	Object::setPosition(x, y, z);
	OnTransformChanged();
}

void BaseNode::setPosition(const Vector3f& position) const {
	Object::setPosition(position);
	OnTransformChanged();	
}

void BaseNode::setOrientation(const Vector3f& axis, float degrees) const {
	Object::setOrientation(axis, degrees);
	OnTransformChanged();
}

void BaseNode::setOrientation(const float degreesX, const float degreesY, const float degreesZ) const {
	Object::setOrientation(degreesX, degreesY, degreesZ);
	OnTransformChanged();
}

void BaseNode::setOrientation(const Vector3f& euler) const {
	Object::setOrientation(euler);
	OnTransformChanged();
}

void BaseNode::setOrientation(const Quaternion& orientation) const {
	Object::setOrientation(orientation);
	OnTransformChanged();
}

void BaseNode::setOrientation(const float x, const float y, const float z, const float w) const {
	Object::setOrientation(x, y, z, w);
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

void BaseNode::rotate(const float x, const float y, const float z, const float w) {
	Object::rotate(x, y, z, w);
	OnTransformChanged();
}

const Vector3f& BaseNode::getWorldOrigin(bool update) const {
	return Vector3f::ZERO;
}