#pragma once

#include "OctreeNode.h"
#include "../BoundingBox.h"
#include "../MeshSequence.h"

class SequenceNode : public OctreeNode {

public:

	SequenceNode(const MeshSequence& meshSequence, int meshIndex);
	using OctreeNode::addChild;
	void addChild(SequenceNode* node, bool drawDebug);
	int getMeshIndex() const;
	const MeshSequence& getSequence() const;

protected:

	const MeshSequence& meshSequence;
	int m_meshIndex;
};