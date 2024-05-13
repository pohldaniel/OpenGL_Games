#include <iostream>
#include "SceneNodeLC.h"

Vector3f SceneNodeLC::WorldPosition;
Vector3f SceneNodeLC::WorldScale;
Quaternion SceneNodeLC::WorldOrientation;

SceneNodeLC::SceneNodeLC() : BaseNode() {
	m_modelMatrix.identity();
}

SceneNodeLC::SceneNodeLC(const SceneNodeLC& rhs) : BaseNode(rhs) {

}

SceneNodeLC::SceneNodeLC(SceneNodeLC&& rhs) : BaseNode(rhs) {

}

SceneNodeLC& SceneNodeLC::operator=(const SceneNodeLC& rhs) {
	BaseNode::operator=(rhs);
	return *this;
}

SceneNodeLC& SceneNodeLC::operator=(SceneNodeLC&& rhs) {
	BaseNode::operator=(rhs);
	return *this;
}

const Matrix4f& SceneNodeLC::getWorldTransformation() const {
	if (m_isDirty) {
		if (m_parent) {			
			m_modelMatrix = static_cast<BaseNode*>(m_parent)->getWorldTransformation() * getTransformationSOP();
		}else {
			m_modelMatrix = getTransformationSOP();
		}
		m_isDirty = false;
	}

	return m_modelMatrix;
}

void SceneNodeLC::updateWorldTransformation() const {
	if (m_isDirty) {
		if (m_parent) {
			m_modelMatrix = static_cast<BaseNode*>(m_parent)->getWorldTransformation() * getTransformationSOP();
		}else {
			m_modelMatrix = getTransformationSOP();
		}
		m_isDirty = false;
	}
}

const Vector3f& SceneNodeLC::getWorldPosition(bool update) const {
	if(update)
		WorldPosition = getWorldTransformation().getTranslation();
	return WorldPosition;
}

const Vector3f& SceneNodeLC::getWorldScale(bool update) const {
	if (update)
		WorldScale = getWorldTransformation().getScale();
	return WorldScale;
}

const Quaternion& SceneNodeLC::getWorldOrientation(bool update) const {
	if (update)
		WorldOrientation = Quaternion(getWorldTransformation().getRotation());
	return WorldOrientation;
}