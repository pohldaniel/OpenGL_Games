#pragma once

#include "OctreeNode.h"
#include "../BoundingBox.h"
#include "../MeshSequence.h"

class SequenceNode : public OctreeNode {

public:

	SequenceNode(const MeshSequence& meshSequence, int meshIndex);
	SequenceNode(const MeshSequence& meshSequence, const BoundingBox& localBoundingBox);
	using OctreeNode::addChild;
	void addChild(SequenceNode* node, bool drawDebug);
	int getMeshIndex() const;
	void setMeshIndex(short index);
	const MeshSequence& getSequence() const;
	
protected:

	const MeshSequence& meshSequence;
	int m_meshIndex;
};