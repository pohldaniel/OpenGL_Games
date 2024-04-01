#include "KinematicObject.h"

KinematicObject::KinematicObject() {

}

KinematicObject::~KinematicObject() {

}

void KinematicObject::create(btCollisionShape* shape, const btTransform& transform, btDynamicsWorld* physicsWorld, int collisionFilterGroup, int collisionFilterMask, void* userPointer) {
	
	btVector3 localInertia(0.0f, 0.0f, 0.0f);


	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo cInfo(0.0f, motionState, shape, localInertia);
	m_rigidBody = new btRigidBody(cInfo);
	m_rigidBody->setUserPointer(userPointer);
	m_rigidBody->forceActivationState(DISABLE_DEACTIVATION);
	m_rigidBody->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);

	physicsWorld->addRigidBody(m_rigidBody, collisionFilterGroup, collisionFilterMask);
}

btRigidBody* KinematicObject::getRigidBody() {
	return m_rigidBody;
}

void KinematicObject::setUserPointer(void* userPointer) {
	m_rigidBody->setUserPointer(userPointer);
}