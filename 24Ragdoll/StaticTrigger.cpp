#include "StaticTrigger.h"

StaticTrigger::StaticTrigger() {

}

StaticTrigger::~StaticTrigger() {

}

void StaticTrigger::create(btCollisionShape* shape, const btTransform& transform, btDynamicsWorld* physicsWorld, int collisionFilterGroup, int collisionFilterMask, void* rigidBodyUserPointer) {
	m_collisionObject = new btCollisionObject();

	m_collisionObject->setCollisionShape(shape);
	m_collisionObject->setWorldTransform(transform);

	m_collisionObject->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	m_collisionObject->forceActivationState(DISABLE_DEACTIVATION);

	physicsWorld->addCollisionObject(m_collisionObject, collisionFilterGroup, collisionFilterMask);
}

btCollisionObject* StaticTrigger::getCollisionObject() {
	return m_collisionObject;
}