#include <iostream>
#include "BoneNode.h"

BoneNode::BoneNode() : m_animationEnabled(true), m_numChildBones(0), m_isRootBone(false) {

}

BoneNode::~BoneNode() {

}

void BoneNode::setAnimationEnabled(bool enable) {
	m_animationEnabled = enable;
}

void BoneNode::countChildBones() {
	m_numChildBones = 0;

	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		if (dynamic_cast<BoneNode*>((*it).get()))
			++m_numChildBones;
	}
}

void BoneNode::OnTransformChanged() {
	SceneNodeLC::OnTransformChanged();
}

void BoneNode::setIsRootBone(bool rootBone) {
	m_isRootBone = rootBone;
}

const bool BoneNode::isRootBone() const {
	return m_isRootBone;
}

bool BoneNode::animationEnabled() const { 
	return m_animationEnabled;
}

size_t BoneNode::getNumChildBones() const { 
	return m_numChildBones;
}

void BoneNode::setTransformSilent(const Vector3f& position, const Quaternion& rotation, const Vector3f& scale) {
	m_position = position;
	m_orientation = rotation;
	m_scale = scale;
}