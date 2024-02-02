#include <GL/glew.h>
#include <iostream>
#include "SceneNode.h"

SceneNode::SceneNode() : BaseNode() {

}

const Vector3f& SceneNode::getPosition() const {	
	updateSOP();
	return m_worldPosition;
}

const Vector3f& SceneNode::getScale() const {
	updateSOP();
	return m_worldScale;
}

const Vector3f& SceneNode::getScalePosition() const {
	return m_scalePosition;
}

const Quaternion& SceneNode::getOrientation() const {
	updateSOP();
	return m_worldOrientation;
}

const Vector3f& SceneNode::getLocalPosition() const {
	return BaseNode::m_position;
}

const Vector3f& SceneNode::getLocalScale() const {
	return BaseNode::m_scale;
}

const Quaternion& SceneNode::getLocalOrientation() const {
	return BaseNode::m_orientation;
}

void SceneNode::updateSOP() const {
	if (m_isDirty) {
		if (m_parent) {
			m_worldPosition = m_parent->getPosition() + BaseNode::m_position;
			m_scalePosition = m_parent->getScalePosition() + BaseNode::m_position * m_parent->getScale();
			m_worldScale = m_parent->getScale() * BaseNode::m_scale;
			m_worldOrientation = m_parent->getOrientation() * BaseNode::m_orientation;
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
	Transformation.translate(getPosition());
	Transformation *= Matrix4f::Rotate(getOrientation(), BaseNode::m_position - getPosition());
	Transformation *= Matrix4f::Scale(getScale(), getScalePosition() - getPosition());
	return Transformation;
}

/*const Matrix4f& SceneNode::getTransformationOld() const {
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
}*/