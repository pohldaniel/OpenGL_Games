#include "Beta.h"

Beta::Beta(const CrowdAgent& crowdAgent, AnimationNode* animationNode) : CrowdAgentEntity(crowdAgent, animationNode){
	m_animationController = new AnimationController(animationNode);
	m_animationController->playExclusive("beta_idle", 0, true, 0.0f);
}

void Beta::draw() {
	m_animationController->getAnimationNode()->drawRaw();
}

void Beta::update(const float dt) {
	m_animationController->update(dt);
	m_animationController->getAnimationNode()->update(dt);
}

void Beta::OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) {
	CrowdAgentEntity::OnPositionVelocityUpdate(pos, vel);
	m_animationController->playExclusive("beta_run", 0, true, 0.1f);
}

void Beta::OnInactive() {
	CrowdAgentEntity::OnInactive();
	m_animationController->playExclusive("beta_idle", 0, true, 0.5f);
}