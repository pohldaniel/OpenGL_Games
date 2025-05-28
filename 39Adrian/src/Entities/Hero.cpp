#include "Hero.h"

Hero::Hero(const CrowdAgent& crowdAgent, Md2Node* md2Node) : CrowdAgentEntity(crowdAgent, md2Node), m_md2Node(md2Node){

}

Hero::~Hero() {

}

void Hero::update(const float dt) {
	m_md2Node->update(dt);
}

void Hero::OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) {
	CrowdAgentEntity::OnPositionVelocityUpdate(pos, vel);
	m_md2Node->setAnimationType(AnimationType::RUN);
}

void Hero::OnInactive() {
	CrowdAgentEntity::OnInactive();
	m_md2Node->setAnimationType(AnimationType::STAND);
}
