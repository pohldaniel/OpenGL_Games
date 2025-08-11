#include "SequenceNode.h"
#include "../DebugRenderer.h"

SequenceNode::SequenceNode(const MeshSequence& meshSequence, short meshIndex) : OctreeNode(),
	meshSequence(meshSequence), 
	m_meshIndex(meshIndex),
	m_speed(1.0f),
	m_activeFrame(0.0f),
	m_materialIndex(-1),
	m_textureIndex(-1) {
	OnBoundingBoxChanged();
}

void SequenceNode::drawRaw(bool force) const {
	meshSequence.draw(m_meshIndex, m_textureIndex, m_materialIndex);
}

void SequenceNode::update(const float dt) {
	if (frameNumber && wasInView(*frameNumber)) {

		m_activeFrame += m_speed * dt;

		int len = static_cast<int>(meshSequence.getMeshes().size()) - 1;
		float lenf = static_cast<float>(len);

		if (m_activeFrame >= lenf)
			m_activeFrame = std::modf(m_activeFrame, &m_activeFrame) + std::fmod(m_activeFrame, lenf + 1.0f);

		m_meshIndex = static_cast<short>(m_activeFrame);
	}
}

short SequenceNode::getMeshIndex() const {
	return m_meshIndex;
}

void SequenceNode::setMeshIndex(short index) {
	m_meshIndex = index;
}

const MeshSequence& SequenceNode::getSequence() const {
	return meshSequence;
}

void SequenceNode::addChild(SequenceNode* node, bool drawDebug) {
	OctreeNode::addChild(node);
	node->setDrawDebug(drawDebug);
}

const BoundingBox& SequenceNode::getLocalBoundingBox() const {
	return meshSequence.getLocalBoundingBox(m_meshIndex);
}

short SequenceNode::getMaterialIndex() const {
	return m_materialIndex;
}

void SequenceNode::setMaterialIndex(short index) {
	m_materialIndex = index;
}

short SequenceNode::getTextureIndex() const {
	return m_textureIndex;
}

void SequenceNode::setTextureIndex(short index) {
	m_textureIndex = index;
}