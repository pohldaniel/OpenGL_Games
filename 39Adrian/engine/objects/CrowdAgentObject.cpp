#include "../scene/SceneNodeLC.h"
#include "CrowdAgentObject.h"

CrowdAgentObject::CrowdAgentObject(const CrowdAgent& crowdAgent, SceneNodeLC* controlledNode) : crowdAgent(crowdAgent), m_controlledNode(controlledNode){

	crowdAgent.setOnPositionVelocityUpdate(std::bind(&CrowdAgentObject::OnPositionVelocityUpdate, this, std::placeholders::_1, std::placeholders::_2));
	crowdAgent.setOnInactive(std::bind(&CrowdAgentObject::OnInactive, this));
	crowdAgent.setOnTarget(std::bind(&CrowdAgentObject::OnTarget, this, std::placeholders::_1));
	crowdAgent.setOnAddAgent(std::bind(&CrowdAgentObject::OnAddAgent, this, std::placeholders::_1));
}

void CrowdAgentObject::setControlledNode(SceneNodeLC* controlledNode) {
	m_controlledNode = controlledNode;
}

void CrowdAgentObject::setPosition(const Vector3f& position) {
	m_controlledNode->setPosition(position);
}

void CrowdAgentObject::setOrientation(const Vector3f& directionXZ) {
	m_controlledNode->getOrientation().set(directionXZ);
}

void CrowdAgentObject::translate(const Vector3f& trans) {
	m_controlledNode->translate(trans);
}

void CrowdAgentObject::translateRelative(const Vector3f& trans) {
	m_controlledNode->translateRelative(trans);
}