#include "SequenceNode.h"
#include "../DebugRenderer.h"

SequenceNode::SequenceNode(const MeshSequence& meshSequence, int meshIndex) : OctreeNode(meshSequence.getLocalBoundingBox(meshIndex)), meshSequence(meshSequence), m_meshIndex(meshIndex) {
	OnBoundingBoxChanged();
}

SequenceNode::SequenceNode(const MeshSequence& meshSequence, const BoundingBox& localBoundingBox) : OctreeNode(localBoundingBox), meshSequence(meshSequence), m_meshIndex(0) {
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