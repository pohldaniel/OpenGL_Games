#include <GL/glew.h>
#include <iostream>
#include "SceneNode.h"

SceneNode::SceneNode() : BaseNode(), m_isSubroot(false) {

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
			m_worldPosition = m_parent->getWorldPosition() + BaseNode::m_position * m_parent->getWorldScale();
			m_worldOrigin = (m_parent->getWorldOrigin() + BaseNode::m_position * m_parent->getWorldScale()) * !m_isSubroot;
			m_worldScale = m_parent->getWorldScale() * BaseNode::m_scale;
			m_worldOrientation = m_parent->getWorldOrientation() * BaseNode::m_orientation;
		}else {
			m_worldPosition = BaseNode::m_position;
			m_worldScale = BaseNode::m_scale;
			m_worldOrientation = BaseNode::m_orientation;
			m_worldOrigin = Vector3f(0.0f);
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