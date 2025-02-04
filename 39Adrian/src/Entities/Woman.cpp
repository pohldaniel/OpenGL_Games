#include "Woman.h"

Woman::Woman(const CrowdAgent& crowdAgent, AnimationNode* animationNode) : CrowdAgentEntity(crowdAgent, animationNode) {
	m_animationController = new AnimationController(animationNode);
	m_animationController->playExclusive("woman_idle", 0, true, 0.0f);
}

Woman::~Woman() {
	delete m_animationController;
}

void Woman::update(const float dt) {
	m_animationController->update(dt);
	m_animationController->getAnimationNode()->update(dt);
}

void Woman::OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) {
	CrowdAgentEntity::OnPositionVelocityUpdate(pos, vel);
	m_animationController->playExclusive("woman_run", 0, true, 0.1f);
}

void Woman::OnInactive() {
	CrowdAgentEntity::OnInactive();
	m_animationController->playExclusive("woman_idle", 0, true, 0.5f);
}

void Woman::setUpdateSilent(bool value) {
	m_animationController->getAnimationNode()->setUpdateSilent(value);
}