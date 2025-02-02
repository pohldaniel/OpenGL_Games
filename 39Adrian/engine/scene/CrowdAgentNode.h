#pragma once

#include "../Navigation/CrowdAgent.h"
#include "../BoundingBox.h"
#include "OctreeNode.h"

class CrowdAgentNode : public OctreeNode {

public:

	CrowdAgentNode(const CrowdAgent& crowdAgent, const OctreeNode& octreeNode);

protected:

	const BoundingBox& getLocalBoundingBox() const;

	short m_materialIndex;
	short m_textureIndex;

private:

	const CrowdAgent& crowdAgent;
	const OctreeNode& octreeNode;
};