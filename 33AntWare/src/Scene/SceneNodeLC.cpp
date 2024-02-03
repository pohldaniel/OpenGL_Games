#include <iostream>
#include "SceneNodeLC.h"

SceneNodeLC::SceneNodeLC() : BaseNode() {
	m_modelMatrix.identity();
}

const Matrix4f& SceneNodeLC::getTransformation() const {
	if (m_isDirty) {
		if (m_parent) {			
			m_modelMatrix = m_parent->getTransformation() * getTransformationSOP();
		}else {
			m_modelMatrix = getTransformationSOP();
		}
		m_isDirty = false;
	}

	return m_modelMatrix;
}