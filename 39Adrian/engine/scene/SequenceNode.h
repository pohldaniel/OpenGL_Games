#pragma once

#include "../BoundingBox.h"
#include "../MeshSequence.h"
#include "OctreeNode.h"

class SequenceNode : public OctreeNode {

public:

	SequenceNode(const MeshSequence& meshSequence, int meshIndex = 0);
	using OctreeNode::addChild;
	void addChild(SequenceNode* node, bool drawDebug);
	int getMeshIndex() const;
	void setMeshIndex(short index);
	const MeshSequence& getSequence() const;
	
protected:

	const BoundingBox& getLocalBoundingBox() const;

	const MeshSequence& meshSequence;
	int m_meshIndex;
};