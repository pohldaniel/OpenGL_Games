#include "CrowdAgentEntity.h"

CrowdAgentEntity::CrowdAgentEntity(const CrowdAgent& crowdAgent, AnimationNode* animationNode) : CrowdAgentNode(crowdAgent), Entity(){
	m_animationController = new AnimationController(animationNode);
	m_animationController->playExclusive("beta_idle", 0, true, 0.0f);
}

void CrowdAgentEntity::draw() {

}

void CrowdAgentEntity::update(const float dt) {
	m_animationController->update(dt);
	m_animationController->getAnimationNode()->update(dt);
	//m_animationNode->update(dt);
}

void CrowdAgentEntity::OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) {
	m_animationController->playExclusive("beta_run", 0, true, 0.1f);
	//getOrientation().set(-vel);
	//setPosition(pos);

	m_animationController->getAnimationNode()->getOrientation().set(-vel);
	m_animationController->getAnimationNode()->setPosition(pos);
}

void CrowdAgentEntity::OnInactive() {
	m_animationController->playExclusive("beta_idle", 0, true, 0.5f);
}