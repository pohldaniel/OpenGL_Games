#include <iostream>
#include "SceneNode.h"

SceneNode::SceneNode() : BaseNode() {

}

SceneNode::SceneNode(const SceneNode& rhs) : BaseNode(rhs){

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

SceneNode::~SceneNode() {

}

const Vector3f& SceneNode::getWorldPosition() const {
	updateSOP();
	return m_worldPosition;
}

const Vector3f& SceneNode::getWorldScale() const {
	updateSOP();
	return m_worldScale;
}

const Vector3f& SceneNode::getWorldOrigin() const {
	updateSOP();
	return m_worldOrigin;
}

const Quaternion& SceneNode::getWorldOrientation() const {
	updateSOP();
	return m_worldOrientation;
}

void SceneNode::updateSOP() const {
	if (m_isDirty) {
		if (m_parent) {
			m_worldPosition = static_cast<BaseNode*>(m_parent)->getWorldPosition() + BaseNode::m_position * static_cast<BaseNode*>(m_parent)->getWorldScale();
			m_worldOrigin = static_cast<BaseNode*>(m_parent)->getWorldOrigin() + BaseNode::m_origin * static_cast<BaseNode*>(m_parent)->getWorldScale();
			m_worldScale = static_cast<BaseNode*>(m_parent)->getWorldScale() * BaseNode::m_scale;
			m_worldOrientation = static_cast<BaseNode*>(m_parent)->getWorldOrientation() * BaseNode::m_orientation;
		}else {
			m_worldPosition = BaseNode::m_position;
			m_worldScale = BaseNode::m_scale;
			m_worldOrientation = BaseNode::m_orientation;
			m_worldOrigin = BaseNode::m_origin;
		}
		m_isDirty = false;
	}
}

const Matrix4f& SceneNode::getWorldTransformation() const {
	Transformation.translate(getWorldPosition());
	Transformation *= Matrix4f::Rotate(m_worldOrientation, -m_worldOrigin);
	Transformation *= Matrix4f::Scale(m_worldScale);
	return Transformation;
}


