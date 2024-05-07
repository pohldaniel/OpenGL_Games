#include "MeshNode.h"
#include "../DebugRenderer.h"

MeshNode::MeshNode(const AssimpModel& model) : OctreeNode(model.getAABB()), model(model) {
	OnBoundingBoxChanged();
}

void MeshNode::addChild(MeshNode* node, bool drawDebug) {
	SceneNodeLC::addChild(node);
}

const AssimpModel& MeshNode::getModel() const {
	return model;
}