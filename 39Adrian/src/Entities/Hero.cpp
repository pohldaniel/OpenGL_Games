#include <Physics/Shapedrawer.h>
#include "Hero.h"

Hero::Hero(const CrowdAgent& crowdAgent, Md2Node* md2Node) : CrowdAgentEntity(crowdAgent, md2Node), Md2Entity(md2Node->getMd2Model()), m_md2Node(md2Node), m_rigidBody(nullptr) {

}

Hero::~Hero() {

}

void Hero::fixedUpdate(float fdt) {
	const BoundingBox& aabb = m_md2Node->getLocalBoundingBox();
	const Vector3f& pos = m_md2Node->getWorldPosition();
	const Quaternion& rot = m_md2Node->getWorldOrientation();
	const Vector3f size = aabb.getSize();
	Vector3f pivot(0.0f, aabb.min[1] + size[1] * 0.5f, 0.0f);
	btTransform trans = Physics::BtTransform(pos + pivot, rot);

	m_rigidBody->setWorldTransform(trans);
	//m_rigidBody->setInterpolationWorldTransform(trans);
	m_rigidBody->getCollisionShape()->setLocalScaling(Physics::VectorFrom(size * 0.75));
	//Physics::GetDynamicsWorld()->updateSingleAabb(m_rigidBody);
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

void Hero::setRigidBody(btRigidBody* rigidBody) {
	m_rigidBody = rigidBody;
	ShapeDrawer::Get().addToCache(m_rigidBody->getCollisionShape());
}

btRigidBody* Hero::getRigidBody() {
	return m_rigidBody;
}