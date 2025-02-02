#include "MeshNode.h"
#include "../DebugRenderer.h"

MeshNode::MeshNode(const AssimpModel& model) : OctreeNode(), model(model) {
	OnBoundingBoxChanged();
}

void MeshNode::addChild(MeshNode* node, bool drawDebug) {
	OctreeNode::addChild(node);
	node->setDrawDebug(drawDebug);
}

const AssimpModel& MeshNode::getModel() const {
	return model;
}

const BoundingBox& MeshNode::getLocalBoundingBox() const {
	return model.getAABB();
}