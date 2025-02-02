#include "CrowdAgentNode.h"

CrowdAgentNode::CrowdAgentNode(const CrowdAgent& crowdAgent, const OctreeNode& octreeNode) :
	OctreeNode(),
	crowdAgent(crowdAgent),
	octreeNode(octreeNode),
	m_materialIndex(-1),
	m_textureIndex(-1) {
}

const BoundingBox& CrowdAgentNode::getLocalBoundingBox() const {
	return octreeNode.getLocalBoundingBox();
}