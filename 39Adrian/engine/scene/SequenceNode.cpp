#include "SequenceNode.h"
#include "../DebugRenderer.h"

SequenceNode::SequenceNode(const MeshSequence& meshSequence, int meshIndex) : OctreeNode(), meshSequence(meshSequence), m_meshIndex(meshIndex) {
	OnBoundingBoxChanged();
}

int SequenceNode::getMeshIndex() const {
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