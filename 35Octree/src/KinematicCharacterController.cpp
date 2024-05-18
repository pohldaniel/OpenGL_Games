#include <iostream>
#include "KinematicCharacterController.h"

const float STEP_HEIGHT = 0.65f;
const float JUMP_HEIGHT = 4.0f;
const float FALL_SPEED = 55.0f;
const float JUMP_SPEED = 12.0f;
const float MAX_SLOPE = 45.0f;
const float DEFAULT_DAMPING = 0.2f;
const Vector3f KINEMATIC_GRAVITY(0.0f, -14.0f, 0.0f);

KinematicCharacterController::KinematicCharacterController(btConvexShape* shape, int collisionFilterGroup, int collisionFilterMask)
	: m_colFilter(collisionFilterGroup)
	, m_colMask(collisionFilterMask)
	, m_gravity(KINEMATIC_GRAVITY)
	, m_stepHeight(STEP_HEIGHT)
	, m_maxJumpHeight(JUMP_HEIGHT)
	, m_fallSpeed(FALL_SPEED)
	, m_jumpSpeed(JUMP_SPEED)
	, m_maxSlope(MAX_SLOPE)
	, m_linearDamping(DEFAULT_DAMPING)
	, m_angularDamping(DEFAULT_DAMPING)
	, m_colShapeOffset(Vector3f::ZERO)
	, m_reapplyAttributes(false),
	m_collisionShape(shape ? shape : new btCapsuleShape(0.7f * 0.5f, std::max(1.8f - 0.7f, 0.0f))){

	m_pairCachingGhostObject = std::make_unique<btPairCachingGhostObject>();
	m_pairCachingGhostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	m_position = Vector3f(28.0f, 8.84f, -4.0f);
	m_rotation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

	physicsWorld_ = Physics::GetDynamicsWorld();

	m_ghostPairCallback = new btGhostPairCallback();
	Physics::GetDynamicsWorld()->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(m_ghostPairCallback);

	addKinematicToWorld();
}

KinematicCharacterController::~KinematicCharacterController() {
	releaseKinematic();

	// Delete GhostPair callback
	if (m_ghostPairCallback) {
		delete m_ghostPairCallback;
		m_ghostPairCallback = nullptr;
	}
}

void KinematicCharacterController::addKinematicToWorld(){
	if (physicsWorld_){
		if (!m_kinematicCharacterController){
			m_pairCachingGhostObject->setCollisionShape(m_collisionShape);			
			m_kinematicCharacterController.reset(new btKinematicCharacterController(m_pairCachingGhostObject.get(), m_collisionShape, m_stepHeight, btVector3(0.0f, 1.0f, 0.0f)));
			m_colShapeOffset = Vector3f(0.0f, 0.84f, 0.0f);
			// apply default settings
			applySettings();

			btDiscreteDynamicsWorld *phyicsWorld = Physics::GetDynamicsWorld();
			phyicsWorld->addCollisionObject(m_pairCachingGhostObject.get(), m_colFilter, m_colMask);
			phyicsWorld->addAction(m_kinematicCharacterController.get());
		}
	}
}

void KinematicCharacterController::applySettings(bool reapply){
	m_kinematicCharacterController->setGravity(Physics::VectorFrom(m_gravity));
	m_kinematicCharacterController->setLinearDamping(m_linearDamping);
	m_kinematicCharacterController->setAngularDamping(m_angularDamping);
	m_kinematicCharacterController->setStepHeight(m_stepHeight);
	m_kinematicCharacterController->setMaxJumpHeight(m_maxJumpHeight);
	m_kinematicCharacterController->setMaxSlope(PI_ON_180 * m_maxSlope);
	m_kinematicCharacterController->setJumpSpeed(m_jumpSpeed);
	m_kinematicCharacterController->setFallSpeed(m_fallSpeed);

	if (reapply && m_pairCachingGhostObject){
		btDiscreteDynamicsWorld *phyicsWorld = Physics::GetDynamicsWorld();
		phyicsWorld->removeCollisionObject(m_pairCachingGhostObject.get());
		phyicsWorld->addCollisionObject(m_pairCachingGhostObject.get(), m_colFilter, m_colMask);
	}

	setTransform(m_position, m_rotation);
}

void KinematicCharacterController::releaseKinematic(){
	if (m_kinematicCharacterController){
		removeKinematicFromWorld();
	}

	m_kinematicCharacterController.reset();
	m_pairCachingGhostObject.reset();
}

void KinematicCharacterController::removeKinematicFromWorld(){
	if (m_kinematicCharacterController && physicsWorld_){
		physicsWorld_->removeCollisionObject(m_pairCachingGhostObject.get());
		physicsWorld_->removeAction(m_kinematicCharacterController.get());
	}
}

void KinematicCharacterController::setTransform(const Vector3f& position, const Quaternion& rotation){
	btTransform worldTrans;
	worldTrans.setIdentity();
	worldTrans.setRotation(Physics::QuaternionFrom(rotation));
	worldTrans.setOrigin(Physics::VectorFrom(position));
	m_pairCachingGhostObject->setWorldTransform(worldTrans);
}

const Vector3f& KinematicCharacterController::getPosition() const {
	btTransform t = m_pairCachingGhostObject->getWorldTransform();
	m_position = Physics::VectorFrom(t.getOrigin()) - m_colShapeOffset;
	return m_position;
}

const btVector3 KinematicCharacterController::getBtPosition() const {
	btTransform t = m_pairCachingGhostObject->getWorldTransform();
	return t.getOrigin();
}

const Quaternion& KinematicCharacterController::getRotation() const {
	btTransform t = m_pairCachingGhostObject->getWorldTransform();
	m_rotation = Physics::QuaternionFrom(t.getRotation());
	return m_rotation;
}

const btQuaternion KinematicCharacterController::getBtRotation() const {
	btTransform t = m_pairCachingGhostObject->getWorldTransform();
	return t.getRotation();
}

const btTransform& KinematicCharacterController::getTransform() const {
	return m_pairCachingGhostObject->getWorldTransform();
}

void KinematicCharacterController::getTransform(Vector3f& position, Quaternion& rotation) {
	btTransform worldTrans = m_pairCachingGhostObject->getWorldTransform();
	rotation = Physics::QuaternionFrom(worldTrans.getRotation());
	position = Physics::VectorFrom(worldTrans.getOrigin());
}

bool KinematicCharacterController::onGround() const {
	return m_kinematicCharacterController->onGround();
}

void KinematicCharacterController::setWalkDirection(const Vector3f& walkDir) {
	m_kinematicCharacterController->setWalkDirection(Physics::VectorFrom(walkDir));
}

void KinematicCharacterController::jump(const Vector3f&jump) {
	m_kinematicCharacterController->jump(Physics::VectorFrom(jump));
}

void KinematicCharacterController::debugDrawContacts() {

	btManifoldArray	manifoldArray;
	btBroadphasePairArray& pairArray = m_pairCachingGhostObject->getOverlappingPairCache()->getOverlappingPairArray();
	int numPairs = pairArray.size();
	std::cout << "Num Pairs: " << numPairs << std::endl;
	for (int i = 0; i < numPairs; i++) {
		manifoldArray.clear();

		const btBroadphasePair& pair = pairArray[i];

		btBroadphasePair* collisionPair = physicsWorld_->getBroadphase()->getOverlappingPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1);
		if (!collisionPair)
			continue;

		if (collisionPair->m_algorithm)
			collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

		for (int j = 0; j < manifoldArray.size(); j++) {
			btPersistentManifold* manifold = manifoldArray[j];
			for (int p = 0; p < manifold->getNumContacts(); p++) {
				const btManifoldPoint&pt = manifold->getContactPoint(p);
				btVector3 color(255, 255, 255);
				physicsWorld_->getDebugDrawer()->drawContactPoint(pt.getPositionWorldOnB(), pt.m_normalWorldOnB, pt.getDistance(), pt.getLifeTime(), color);
			}
		}
	}
}

void KinematicCharacterController::setUserPointer(void* userPointer) {
	m_pairCachingGhostObject.get()->setUserPointer(userPointer);
}

void KinematicCharacterController::setPosition(const Vector3f& position) {
	m_position = position;
	setTransform(m_position, m_rotation);
}

void KinematicCharacterController::setLinearVelocity(const Vector3f& velocity) {
	m_kinematicCharacterController->setLinearVelocity(Physics::VectorFrom(velocity));
}

const Vector3f KinematicCharacterController::getLinearVelocity() const{
	return Physics::VectorFrom(m_kinematicCharacterController->getLinearVelocity());
}

btPairCachingGhostObject* KinematicCharacterController::getPairCachingGhostObject() const{
	return m_pairCachingGhostObject.get();
}