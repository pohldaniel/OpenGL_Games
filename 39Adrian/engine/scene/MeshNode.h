#pragma once

#include "OctreeNode.h"
#include "../BoundingBox.h"
#include "../AssimpModel.h"

class MeshNode : public OctreeNode {

public:

	MeshNode(const AssimpModel& model);
	~MeshNode() = default;

	using OctreeNode::addChild;
	void addChild(MeshNode* node, bool drawDebug);
	const AssimpModel& getModel() const;

protected:

	const AssimpModel& model;
};