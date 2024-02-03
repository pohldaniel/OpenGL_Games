#include <GL/glew.h>
#include <iostream>
#include "SceneNode.h"

SceneNode::SceneNode() : BaseNode() {

}

const Vector3f& SceneNode::getWorldPosition() const {
	updateSOP();
	return m_worldPosition;
}

const Vector3f& SceneNode::getWorldScale() const {
	updateSOP();
	return m_worldScale;
}

const Vector3f& SceneNode::getScalePosition() const {
	return m_scalePosition;
}

const Quaternion& SceneNode::getWorldOrientation() const {
	updateSOP();
	return m_worldOrientation;
}

void SceneNode::updateSOP() const {
	if (m_isDirty) {
		if (m_parent) {
			m_worldPosition = m_parent->getWorldPosition() + BaseNode::m_position;
			m_scalePosition = m_parent->getScalePosition() + BaseNode::m_position * m_parent->getScale();
			m_worldScale = m_parent->getWorldScale() * BaseNode::m_scale;
			m_worldOrientation = m_parent->getWorldOrientation() * BaseNode::m_orientation;
		}else {
			m_worldPosition = BaseNode::m_position;
			m_scalePosition = BaseNode::m_position;
			m_worldScale = BaseNode::m_scale;
			m_worldOrientation = BaseNode::m_orientation;

		}
		m_isDirty = false;
	}
}

const Matrix4f& SceneNode::getTransformation() const {
	//Transformation.translate(getPosition());
	//Transformation *= Matrix4f::Rotate(m_worldOrientation, BaseNode::m_position - m_worldPosition);
	//Transformation *= Matrix4f::Scale(getScale(), m_scalePosition - m_worldPosition);
	//return Transformation;

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