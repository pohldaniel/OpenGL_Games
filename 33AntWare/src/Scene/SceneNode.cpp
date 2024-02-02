#include <iostream>
#include "SceneNode.h"

SceneNode::SceneNode() : ObjectNode() {
	m_modelMatrix.identity();
}

const Matrix4f& SceneNode::getTransformation() const {

	if (m_isDirty) {
		if (m_parent) {
			m_modelMatrix = m_parent->getTransformation() * getTransformationSOP();
		}
		else {
			m_modelMatrix = getTransformationSOP();
		}
		m_isDirty = false;
	}

	return m_modelMatrix;
}

const Vector3f& SceneNode::getPosition() const {
	return m_position;
}

const Vector3f& SceneNode::getScale() const {
	return m_scale;
}

const Quaternion& SceneNode::getOrientation() const {
	return m_orientation;
}