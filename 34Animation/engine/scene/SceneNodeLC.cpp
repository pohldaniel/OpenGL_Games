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
		//std::cout << "Parent: " << m_parent << std::endl;
		if (m_parent) {			
			m_modelMatrix = m_parent->getWorldTransformation() * getTransformationSOP();
		}else {
			m_modelMatrix = getTransformationSOP();
		}
		m_isDirty = false;
	}

	return m_modelMatrix;
}

const Vector3f& SceneNodeLC::getWorldPosition() const {
	WorldPosition = getWorldTransformation().getTranslation();
	return WorldPosition;
}

const Vector3f& SceneNodeLC::getWorldScale() const {
	WorldScale = getWorldTransformation().getScale();
	return WorldScale;
}

const Quaternion& SceneNodeLC::getWorldOrientation() const {
	WorldOrientation = Quaternion(getWorldTransformation().getRotation());
	return WorldOrientation;
}