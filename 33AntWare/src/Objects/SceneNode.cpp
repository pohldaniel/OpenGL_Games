#include <iostream>
#include "SceneNode.h"

SceneNode::SceneNode() : Object(), m_parent(nullptr), m_markForRemove(false){
	m_modelMatrix.identity();
	m_isDirty = true;
}

/*SceneNode::SceneNode(SceneNode const& rhs) : Object(rhs) {

}

SceneNode& SceneNode::operator=(const SceneNode& rhs) {
	Object::operator=(rhs);
	return *this;
}*/

void SceneNode::removeChild(std::unique_ptr<SceneNode> node){
	m_children.remove(node);
}

const std::list<std::unique_ptr<SceneNode>>& SceneNode::getChildren() const {
	return m_children;
}

void SceneNode::markForRemove() {
	m_markForRemove = true;
}

void SceneNode::addChild(SceneNode* node) {
	m_children.emplace_back(std::unique_ptr<SceneNode>(node));
	m_children.back()->m_parent = this;
}


void SceneNode::addChild() {
	m_children.emplace_back(std::make_unique<SceneNode>());
	m_children.back()->m_parent = this;
}

const Matrix4f& SceneNode::getTransformation() {

	if (m_isDirty) {
		if (m_parent) {
			m_modelMatrix = m_parent->getTransformation() * getTransformationSOP();
		}else {
			m_modelMatrix = getTransformationSOP();
		}
		m_isDirty = false;
	}

	return m_modelMatrix;
}

void SceneNode::OnTransformChanged() {

	if (m_isDirty) {
		return;
	}

	for(auto&& child : m_children) {
		child->OnTransformChanged();
	}

	m_isDirty = true;
}

void SceneNode::setScale(const float sx, const float sy, const float sz) {
	Object::setScale(sx, sy, sz);
	OnTransformChanged();
}

void SceneNode::setScale(const Vector3f& scale) {
	Object::setScale(scale);
	OnTransformChanged();
}

void SceneNode::setScale(const float s) {
	Object::setScale(s);
	OnTransformChanged();
}

void SceneNode::setPosition(const float x, const float y, const float z) {
	Object::setPosition(x, y, z);
	OnTransformChanged();
}

void SceneNode::setPosition(const Vector3f& position) {
	Object::setPosition(position);
	OnTransformChanged();
}

void SceneNode::setOrientation(const Vector3f& axis, float degrees) {
	Object::setOrientation(axis, degrees);
	OnTransformChanged();
}

void SceneNode::setOrientation(const float degreesX, const float degreesY, const float degreesZ) {
	Object::setOrientation(degreesX, degreesY, degreesZ);
	OnTransformChanged();
}

void SceneNode::setOrientation(const Vector3f& euler) {
	Object::setOrientation(euler);
	OnTransformChanged();
}

void SceneNode::setOrientation(const Quaternion& orientation) {
	Object::setOrientation(orientation);
	OnTransformChanged();
}

void SceneNode::translate(const Vector3f& trans) {
	Object::translate(trans);
	OnTransformChanged();
}

void SceneNode::translate(const float dx, const float dy, const float dz) {
	Object::translate(dx, dy, dz);
	OnTransformChanged();
}

void SceneNode::translateRelative(const Vector3f& trans) {
	Object::translateRelative(trans);
	OnTransformChanged();
}

void SceneNode::translateRelative(const float dx, const float dy, const float dz) {
	Object::translateRelative(dx, dy, dz);
	OnTransformChanged();
}

void SceneNode::scale(const Vector3f& scale) {
	Object::scale(scale);
	OnTransformChanged();
}

void SceneNode::scale(const float sx, const float sy, const float sz) {
	Object::scale(sx, sy, sz);
	OnTransformChanged();
}

void SceneNode::scale(const float s) {
	Object::scale(s);
	OnTransformChanged();
}

void SceneNode::rotate(const float pitch, const float yaw, const float roll) {
	Object::rotate(pitch, yaw, roll);
	OnTransformChanged();
}

void SceneNode::rotate(const Vector3f& eulerAngle) {
	Object::rotate(eulerAngle);
	OnTransformChanged();
}

void SceneNode::rotate(const Vector3f& axis, float degrees) {
	Object::rotate(axis, degrees);
	OnTransformChanged();
}

void SceneNode::rotate(const Quaternion& orientation) {
	Object::rotate(orientation);
	OnTransformChanged();
}