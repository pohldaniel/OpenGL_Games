#include "Jack.h"

Jack::Jack(const CrowdAgent& crowdAgent, AnimationNode* animationNode) : CrowdAgentEntity(crowdAgent, animationNode) {
	m_animationController = new AnimationController(animationNode);
}

Jack::~Jack() {
	delete m_animationController;
}

void Jack::update(const float dt) {
	m_animationController->update(dt);
	m_animationController->getAnimationNode()->update(dt);
}

void Jack::OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) {
	CrowdAgentEntity::OnPositionVelocityUpdate(pos, vel);
	playAnimation();
}

void Jack::playAnimation() {
	m_animationController->playExclusive("jack_walk", 0, true, 0.1f);	
}

void Jack::OnInactive() {
	CrowdAgentEntity::OnInactive();
	m_animationController->stop("jack_walk", 0.5f);
}

const Vector3f& Jack::getPosition() {
	return m_controlledNode->getWorldPosition();
}