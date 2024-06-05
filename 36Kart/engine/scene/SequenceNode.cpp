#include "SequenceNode.h"
#include "../DebugRenderer.h"

SequenceNode::SequenceNode(const MeshSequence& meshSequence, int meshIndex) : OctreeNode(meshSequence.getLocalBoundingBox(meshIndex)), meshSequence(meshSequence), m_meshIndex(meshIndex) {
	OnBoundingBoxChanged();
}

int SequenceNode::getMeshIndex() const {
	return m_meshIndex;
}

const MeshSequence& SequenceNode::getSequence() const {
	return meshSequence;
}

void SequenceNode::addChild(SequenceNode* node, bool drawDebug) {
	OctreeNode::addChild(node);
	node->setDrawDebug(drawDebug);
}