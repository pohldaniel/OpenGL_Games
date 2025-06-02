#include <Physics/Shapedrawer.h>
#include "Hero.h"

Hero::Hero(const Md2Model& md2Model, const CrowdAgent& crowdAgent) : CrowdAgentEntity(crowdAgent, this), Md2Entity(md2Model), m_rigidBody(nullptr) {

}

Hero::~Hero() {

}

void Hero::fixedUpdate(float fdt) {
	const BoundingBox& aabb = getLocalBoundingBox();
	const Vector3f& pos = getWorldPosition();
	const Quaternion& rot = getWorldOrientation();
	const Vector3f size = aabb.getSize();
	Vector3f pivot(0.0f, aabb.min[1] + size[1] * 0.5f, 0.0f);
	btTransform trans = Physics::BtTransform(pos + pivot, rot);

	m_rigidBody->setWorldTransform(trans);
	m_rigidBody->setInterpolationWorldTransform(trans);
	m_rigidBody->getCollisionShape()->setLocalScaling(Physics::VectorFrom(size * 0.75));
	Physics::GetDynamicsWorld()->updateSingleAabb(m_rigidBody, true);
}

void Hero::update(const float dt) {
	Md2Node::update(dt);
}

void Hero::OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) {
	CrowdAgentEntity::OnPositionVelocityUpdate(pos, vel);
	setAnimationType(AnimationType::RUN);
}

void Hero::OnInactive() {
	CrowdAgentEntity::OnInactive();
	setAnimationType(AnimationType::STAND);
}

void Hero::setRigidBody(btRigidBody* rigidBody) {
	m_rigidBody = rigidBody;
	ShapeDrawer::Get().addToCache(m_rigidBody->getCollisionShape());
}

btRigidBody* Hero::getRigidBody() {
	return m_rigidBody;
}