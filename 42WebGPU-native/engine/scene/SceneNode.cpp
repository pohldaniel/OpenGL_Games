#include <iostream>
#include "SceneNode.h"

thread_local Vector3f SceneNode::WorldPosition;
thread_local Vector3f SceneNode::WorldScale;
thread_local Quaternion SceneNode::WorldOrientation;

SceneNode::SceneNode() : BaseNode() {
	m_modelMatrix.identity();
}

SceneNode::SceneNode(const SceneNode& rhs) : BaseNode(rhs) {

}

SceneNode::SceneNode(SceneNode&& rhs) : BaseNode(rhs) {

}

SceneNode& SceneNode::operator=(const SceneNode& rhs) {
	BaseNode::operator=(rhs);
	return *this;
}

SceneNode& SceneNode::operator=(SceneNode&& rhs) {
	BaseNode::operator=(rhs);
	return *this;
}

const Matrix4f& SceneNode::getWorldTransformation() const {
	if (m_isDirty) {
		m_modelMatrix = getTransformationSOP();
		if (m_parent)
			m_modelMatrix = static_cast<BaseNode*>(m_parent)->getWorldTransformation() * m_modelMatrix;

		m_isDirty = false;
	}

	return m_modelMatrix;
}

void SceneNode::updateWorldTransformation() const {
	if (m_isDirty) {
		m_modelMatrix = getTransformationSOP();
		if (m_parent)
			m_modelMatrix = static_cast<BaseNode*>(m_parent)->getWorldTransformation() * m_modelMatrix;

		m_isDirty = false;
	}
}

const Vector3f& SceneNode::getWorldPosition(bool update) const {
	if(update)
		WorldPosition = getWorldTransformation().getTranslation();
	return WorldPosition;
}

const Vector3f& SceneNode::getWorldScale(bool update) const {
	if (update)
		WorldScale = getWorldTransformation().getScale();
	return WorldScale;
}

const Quaternion& SceneNode::getWorldOrientation(bool update) const {
	if (update)
		WorldOrientation = Quaternion(getWorldTransformation().getRotation());
	return WorldOrientation;
}