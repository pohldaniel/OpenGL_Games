#include <GL/glew.h>
#include <iostream>
#include "ObjectNode.h"

#include <engine/BuiltInShader.h>

ObjectNode::ObjectNode()  : BaseNode() {

}

const Vector3f& ObjectNode::getPosition() const {
	updateSOP();
	return worldPosition;
}

const Vector3f& ObjectNode::getScale() const {
	updateSOP();
	return worldScale;
}

const Quaternion& ObjectNode::getOrientation() const {
	updateSOP();
	return worldOrientation;
}

void ObjectNode::updateSOP() const {
	if (m_isDirty) {
		if (m_parent) {
			worldPosition = m_parent->getPosition() + m_position;
			worldScale = m_parent->getScale() * m_scale;
			worldOrientation = m_parent->getOrientation() * m_orientation;
		}else {
			worldPosition = m_position;
			worldScale = m_scale;
			worldOrientation = m_orientation;

		}
		m_isDirty = false;
	}
}