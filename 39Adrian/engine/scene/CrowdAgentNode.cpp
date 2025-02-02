#include "CrowdAgentNode.h"

CrowdAgentNode::CrowdAgentNode(const CrowdAgent& crowdAgent) : crowdAgent(crowdAgent){
	crowdAgent.setOnPositionVelocityUpdate(std::bind(&CrowdAgentNode::OnPositionVelocityUpdate, this, std::placeholders::_1, std::placeholders::_2));
	crowdAgent.setOnInactive(std::bind(&CrowdAgentNode::OnInactive, this));
}

void CrowdAgentNode::setControlledNode(SceneNodeLC* controlledNode) {
	m_controlledNode = controlledNode;
}