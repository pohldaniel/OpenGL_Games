#include <iostream>
#include "BaseNode.h"
#include "SceneNode.h"
#include "SceneNodeLC.h"

BaseNode::BaseNode() : Object(), m_markForRemove(false), m_isDirty(true), m_parent(nullptr), m_isFixed(false), m_isSelfCared(false){

}

BaseNode::BaseNode(const BaseNode& rhs) : Object(rhs) {
	m_isDirty = rhs.m_isDirty;
	m_markForRemove = rhs.m_markForRemove;
	m_isFixed = rhs.m_isFixed;
	m_isSelfCared = rhs.m_isSelfCared;
}

BaseNode& BaseNode::operator=(const BaseNode& rhs) {
	Object::operator=(rhs);
	m_isDirty = rhs.m_isDirty;
	m_markForRemove = rhs.m_markForRemove;
	m_isFixed = rhs.m_isFixed;
	m_isSelfCared = rhs.m_isSelfCared;
	return *this;
}

BaseNode::BaseNode(BaseNode&& rhs) : Object(rhs) {
	m_isDirty = rhs.m_isDirty;
	m_markForRemove = rhs.m_markForRemove;
	m_isFixed = rhs.m_isFixed;
	m_isSelfCared = rhs.m_isSelfCared;
}

BaseNode& BaseNode::operator=(BaseNode&& rhs) {
	Object::operator=(rhs);
	m_isDirty = rhs.m_isDirty;
	m_markForRemove = rhs.m_markForRemove;
	m_isFixed = rhs.m_isFixed;
	m_isSelfCared = rhs.m_isSelfCared;
	return *this;
}

BaseNode::~BaseNode() {
	removeAllChildren();
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

void BaseNode::setOrigin(const float x, const float y, const float z) {
	Object::setOrigin(x, y, z);
	OnTransformChanged();
}

void BaseNode::setOrigin(const Vector3f& origin) {
	Object::setOrigin(origin);
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

std::list<std::unique_ptr<BaseNode>>& BaseNode::getChildren() const{
	return m_children;
}

BaseNode* BaseNode::addChild(BaseNode* node) {
	m_children.emplace_back(std::unique_ptr<BaseNode>(node));
	m_children.back()->m_parent = this;
	return m_children.back().get();
}

BaseNode* BaseNode::addChild() {
	m_children.emplace_back(std::make_unique<SceneNode>());
	m_children.back()->m_parent = this;
	return m_children.back().get();
}

const BaseNode* BaseNode::getParent() const {
	return m_parent;
}

void BaseNode::removeAllChildren() {
	for(auto it = m_children.begin(); it != m_children.end(); ++it){
		BaseNode* child = (*it).release();
		child->m_parent = nullptr;
		delete child;	
		child = nullptr;
	}
	m_children.clear();
}

void BaseNode::setParent(BaseNode* node) {

	if (node && m_parent) {	
		std::list<std::unique_ptr<BaseNode>>::iterator it = std::find_if(m_parent->getChildren().begin(), m_parent->getChildren().end(), [node](std::unique_ptr<BaseNode>& _node) { return _node.get() == node; });
		
		if (it != m_parent->getChildren().end()) {
			node->getChildren().splice(m_parent->getChildren().end(), m_parent->getChildren(), it);
		}
	}else if(node){
		node->addChild(this);
	}else if(m_parent){
		m_parent->eraseChild(this);
	}
}

const Vector3f& BaseNode::getWorldOrigin() const {
	return Vector3f::ZERO;
}

void BaseNode::setIsFixed(bool isFixed) {
	m_isFixed = isFixed;
}

const bool BaseNode::isFixed() const{
	return m_isFixed;
}

void BaseNode::setIsSelfCared(bool isSelfCared) {
	m_isSelfCared = isSelfCared;
}

const bool BaseNode::isSelfCared() const {
	return m_isSelfCared;
}

void BaseNode::eraseChild(BaseNode* child) {

	if (!child || child->m_parent != this)
		return;

	child->m_parent = nullptr;
	m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [child](const std::unique_ptr<BaseNode>& node) { return node.get() == child; }), m_children.end());
}

void BaseNode::eraseSelf() {
	if (m_parent)
		m_parent->eraseChild(this);
}

void BaseNode::removeChild(BaseNode* child) {

	if (!child || child->m_parent != this)
		return;

	child->m_parent = nullptr;

	std::remove_if(m_children.begin(), m_children.end(), [child](const std::unique_ptr<BaseNode>& node) -> bool { return node.get() == child; });
}

void BaseNode::removeSelf() {
	if (m_parent)
		m_parent->removeChild(this);
}