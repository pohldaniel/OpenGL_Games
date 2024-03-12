#include <iostream>
#include "BoneNode.h"

BoneNode::BoneNode() : animationEnabled(true), numChildBones(0), m_rootBone(false) {

}

BoneNode::~BoneNode() {

}

void BoneNode::SetAnimationEnabled(bool enable) {
	animationEnabled = enable;
}

void BoneNode::CountChildBones() {
	numChildBones = 0;

	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		if (dynamic_cast<BoneNode*>((*it).get()))
			++numChildBones;
	}
}

void BoneNode::OnTransformChanged() {
	SceneNodeLC::OnTransformChanged();
}

BoneNode* BoneNode::FindChildOfType(StringHash childNameHash, bool recursive) const {
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		BoneNode* child = dynamic_cast<BoneNode*>((*it).get());
		if (!child) {
			continue;
		}

		if (child->nameHash == childNameHash)
			return child;
		else if (recursive && child->m_children.size()) {
			BoneNode* childResult = child->FindChildOfType(childNameHash, recursive);
			if (childResult)
				return childResult;
		}
	}

	return nullptr;
}

void BoneNode::setRootBone(bool rootBone) {
	m_rootBone = rootBone;
}

const bool BoneNode::isRootBone() const {
	return m_rootBone;
}