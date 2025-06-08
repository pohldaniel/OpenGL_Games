#include <engine/MeshObject/Shape.h>
#include <engine/Scene/ShapeNode.h>
#include <Physics/Shapedrawer.h>
#include <States/Adrian.h>
#include "Hero.h"

Hero::Hero(const Md2Model& md2Model, const CrowdAgent& crowdAgent) : CrowdAgentEntity(crowdAgent, this), Md2Entity(md2Model), m_rigidBody(nullptr), m_isDeath(false){
	setRigidBody(Physics::AddKinematicRigidBody(Physics::BtTransform(Physics::VectorFrom(getWorldPosition())), new btBoxShape(btVector3(0.5f, 0.5f, 0.5f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER, this, true));
	ShapeDrawer::Get().addToCache(m_rigidBody->getCollisionShape());
}

Hero::~Hero() {

}

void Hero::fixedUpdate(float fdt) {
	if (isDeath())
		return;

	const BoundingBox& aabb = getLocalBoundingBox();
	const Vector3f& pos = getWorldPosition();
	const Quaternion& rot = getWorldOrientation();
	const Vector3f size = aabb.getSize();
	const Vector3f pivot1(0.0f, aabb.min[1] + size[1] * 0.5f, 0.0f);
	const Vector3f pivot2(0.0f, 0.5f, 0.0f);

	m_rigidBody->getMotionState()->setWorldTransform(Physics::BtTransform(pos + pivot1, rot));
	m_rigidBody->getCollisionShape()->setLocalScaling(Physics::VectorFrom(size * 0.75));
	Physics::GetDynamicsWorld()->updateSingleAabb(m_rigidBody, true);

	m_segmentBody->getMotionState()->setWorldTransform(Physics::BtTransform(pos, rot));
	m_triggerBody->getMotionState()->setWorldTransform(Physics::BtTransform(pos, rot));
}

void Hero::update(const float dt) {
	Md2Node::update(dt);
}

void Hero::init(const Shape& shape) {
	m_segmentBody = Physics::AddKinematicRigidBody(Physics::BtTransform(Physics::VectorFrom(getWorldPosition())), Physics::CreateCollisionShape(&shape, btVector3(1.5f, 1.5f, 1.5f)), Physics::collisiontypes::TRIGGER_1, Physics::collisiontypes::ENEMY, nullptr, true);
	m_triggerBody = Physics::AddKinematicRigidBody(Physics::BtTransform(Physics::VectorFrom(getWorldPosition())), new btCylinderShape(btVector3(0.5f, 1.0f, 0.0f)), Physics::collisiontypes::TRIGGER_2, Physics::collisiontypes::ENEMY, this, true);
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
}

btRigidBody* Hero::getRigidBody() {
	return m_rigidBody;
}

btRigidBody* Hero::getSegmentBody() {
	return m_segmentBody;
}

btRigidBody* Hero::getTriggerBody() {
	return m_triggerBody;
}

void Hero::handleCollision(btCollisionObject* collisionObject) {
	if(!isDeath() && collisionObject)
		Physics::GetDynamicsWorld()->contactPairTest(m_triggerBody, collisionObject, m_triggerResult);
}

void Hero::setIsDeath(bool isDeath) {
	m_isDeath = isDeath;
}

bool Hero::isDeath() {
	return m_isDeath;
}

void Hero::setOnDeath(std::function<void()> fun) {
	m_triggerResult.OnDeath = fun;
}

btScalar Hero::TriggerCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {

	Hero* hero = reinterpret_cast<Hero*>(colObj0Wrap->getCollisionObject()->getUserPointer());
	hero->setAnimationType(AnimationType::DEATH_BACK);
	hero->setLoopAnimation(false);
	hero->resetAgent();
	hero->removeAgent();
	hero->setIsDeath(true);
	

	ShapeNode* shape = hero->findChild<ShapeNode>("disk");
	if (shape) {
		shape->OnOctreeSet(nullptr);
		shape->eraseSelf();
	}

	shape = hero->findChild<ShapeNode>("segment");
	if (shape) {
		shape->OnOctreeSet(nullptr);
		shape->eraseSelf();
	}

	hero->setColor(Vector4f::ONE);
	
	if (hero->getRigidBody()) {
		Physics::DeleteCollisionObject(hero->getRigidBody());
		hero->setRigidBody(nullptr);
	}

	if (OnDeath)
		OnDeath();

	return 0;
}