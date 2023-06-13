#include <iostream>

#include "CharacterController.h"
#include "engine/Vector.h"

const float DEFAULT_STEP_HEIGHT = 0.1f; // The default amount to move the character up before resolving collisions.
const btVector3 UP_VECTOR(0.0f, 1.0f, 0.0f);
const btVector3 ZERO_VECTOR(0.0f, 0.0f, 0.0f);

// Ray/convex result callbacks that ignore the self object passed to them as well as ghost objects.
class RayResultCallback : public btCollisionWorld::ClosestRayResultCallback{

public:
	
	RayResultCallback(btCollisionObject* self) : btCollisionWorld::ClosestRayResultCallback(ZERO_VECTOR, ZERO_VECTOR), mSelf(self) {
		m_collisionFilterGroup = self->getBroadphaseHandle()->m_collisionFilterGroup;
		m_collisionFilterMask = self->getBroadphaseHandle()->m_collisionFilterMask;
	}

	btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace){
		if (rayResult.m_collisionObject == mSelf) return 1.0f;
		return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
	}

private:
	btCollisionObject* mSelf;
};

class ConvexResultCallback : public btCollisionWorld::ClosestConvexResultCallback{

public:
	ConvexResultCallback(btCollisionObject* self, const btVector3& up, btScalar minSlopeDot) : btCollisionWorld::ClosestConvexResultCallback(ZERO_VECTOR, ZERO_VECTOR), mSelf(self) {
		m_collisionFilterGroup = self->getBroadphaseHandle()->m_collisionFilterGroup;
		m_collisionFilterMask = self->getBroadphaseHandle()->m_collisionFilterMask;
	}

	btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace){
		if (convexResult.m_hitCollisionObject == mSelf) return 1.0f;
		return ClosestConvexResultCallback::addSingleResult(convexResult, normalInWorldSpace);
	}

private:
	btCollisionObject* mSelf;
};

CharacterController::CharacterController()
	: m_collisionWorld(nullptr)
	, m_rigidBody(nullptr)
	, m_shape(nullptr)
	, m_onGround(false)
	, m_onSteepSlope(false)
	, m_maxClimbSlopeAngle(0.6f)
	, m_canJump(false)
	, m_jumpTicks(0)
	, m_slopeDot(0.0f)
	, m_slopeNormal(ZERO_VECTOR){

}

CharacterController::~CharacterController(){
	destroy();
}

void CharacterController::create(btRigidBody* rigidBody, btDynamicsWorld* physicsWorld, int collisionFilterGroup, int collisionFilterMask, void* rigidBodyUserPointer) {
	m_collisionWorld = physicsWorld;
	m_shape = rigidBody->getCollisionShape();
	m_rigidBody = rigidBody;

	m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
	m_rigidBody->setUserPointer(rigidBodyUserPointer);

	m_rigidBody->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
	m_rigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
	m_rigidBody->setSleepingThresholds(0.0f, 0.0f);
	m_rigidBody->setDamping(0.0f, 0.0f);
	m_rigidBody->setRollingFriction(0.0f);

	physicsWorld->addRigidBody(m_rigidBody, collisionFilterGroup, collisionFilterMask);
}

void CharacterController::destroy(){
	delete m_rigidBody;
	m_rigidBody = nullptr;

	delete m_shape;
	m_shape = nullptr;

	m_collisionWorld = nullptr;
}

void CharacterController::setSlopeAngle(float degrees) {
	m_maxClimbSlopeAngle = cosf(degrees * PI_ON_180);
}

void CharacterController::setJumpDistanceOffset(float value) {
	m_jumpDistanceOffset = value;
}

void CharacterController::setOnGroundDistanceOffset(float value) {
	m_onGroundDistanceOffset = value;
}

//this values are deppending on the used geometry
float CharacterController::getVelocityWeight(float sloapDot) {
	
	if (m_slopeDot < 0.3f) {
		return m_groundTicks ? 0.0f : 3.3f;
	}if (m_slopeDot < 0.4f) {
		return 2.8f;
	}if (m_slopeDot < 0.5f) {
		return 2.0f;
	}else if (m_slopeDot < 0.6f) {
		return 1.4f;
	}else
		return 1.0f;
}

void CharacterController::preStep(){
	
	if (m_onSteepSlope){
		btVector3 uAxis = m_slopeNormal.cross(UP_VECTOR).normalize();
		btVector3 vAxis = uAxis.cross(m_slopeNormal);
		btVector3 fixVel = (vAxis / m_slopeNormal.dot(UP_VECTOR)) * getVelocityWeight(m_slopeDot);
		//fixVel[1] = m_onGround ? 0.0f : fixVel[1];
		m_rigidBody->setLinearVelocity(-fixVel);
	}
}

void CharacterController::postStep() {
	m_onGround = false;
	m_canJump = false;
	m_falling = false;
	const float TEST_DISTANCE_RAY = 10.0f;
	const float TEST_DISTANCE = 2.0f;

	btVector3 from = m_rigidBody->getWorldTransform().getOrigin();
	btVector3 to = from - btVector3(0, TEST_DISTANCE_RAY, 0);

	// detect ground collision and update the "on ground" status
	RayResultCallback callbackRay(m_rigidBody);
	m_collisionWorld->rayTest(from, to, callbackRay);

	// Check if there is something below the character.
	if (callbackRay.hasHit()){

		btVector3 end = from + (to - from) * callbackRay.m_closestHitFraction;
		btVector3 normal = callbackRay.m_hitNormalWorld;
		float slopeDot = normal.dot(btVector3(0, 1, 0));

		float distance = btDistance(end, from);
		m_onGround = distance < m_onGroundDistanceOffset && slopeDot > 0.97f;

		m_falling = distance > TEST_DISTANCE;

		if (m_onGround) {
			moveCharacterAlongY(std::fabs(distance - 0.5f));
			m_groundTicks = 10;
		}

	}

	btTransform tsFrom, tsTo;
	to = from - btVector3(0.0f, TEST_DISTANCE, 0.0f);

	tsFrom.setIdentity();
	tsFrom.setOrigin(from);

	tsTo.setIdentity();
	tsTo.setOrigin(to);

	// detect slopes and jump condition
	ConvexResultCallback callbackConv(m_rigidBody, UP_VECTOR, 0.01f);
	m_collisionWorld->convexSweepTest((btConvexShape*)m_shape, tsFrom, tsTo, callbackConv, m_collisionWorld->getDispatchInfo().m_allowedCcdPenetration);
	
	if (callbackConv.hasHit()){

		btVector3 end = from + (to - from) * callbackConv.m_closestHitFraction;
		btVector3 normal = callbackConv.m_hitNormalWorld;
		
		// slope test
		m_slopeDot = normal.dot(btVector3(0.0f, 1.0f, 0.0f));
		m_onSteepSlope = (m_slopeDot < m_maxClimbSlopeAngle) && !m_falling && m_jumpTicks == 0;
		m_slopeNormal = normal;

		// Compute distance to the floor.
		float distance = btDistance(end, from);
		m_canJump = distance < m_jumpDistanceOffset && m_jumpTicks == 0 && !m_onSteepSlope;
		
	}

	m_jumpTicks = m_jumpTicks > 0 ? m_jumpTicks -= 1 : 0;
	m_groundTicks = m_groundTicks > 0 ? m_groundTicks -= 1 : 0;
}

bool CharacterController::onGround() const{
	return m_onGround;
}

//this values are deppending on the used geometry
unsigned short CharacterController::getJumpTicks(float sloapDot) {

	if (m_slopeDot < 0.3f) {
		return 4;
	}else if (m_slopeDot < 0.4f)
		return 6;
	else {
		return 10;
	}
}

void CharacterController::jump(const btVector3& direction, float force){
	
	if (m_canJump){
		//reduce rejump ticks on steep slopes
		m_jumpTicks = getJumpTicks(m_slopeDot);
		m_canJump = false;
		m_rigidBody->setLinearVelocity(direction * force);
	}
}

void CharacterController::applyCentralImpulse(const btVector3& direction) {
	m_rigidBody->applyCentralImpulse(direction);
}

void CharacterController::setLinearVelocity(const btVector3& vel){
	m_rigidBody->setLinearVelocity(vel);
}

void CharacterController::setAngularVelocity(const btVector3& angVel) {
	m_rigidBody->setAngularVelocity(angVel);
}

void CharacterController::setLinearVelocityXZ(const btVector3& vel) {
	
	btVector3 _vel = m_rigidBody->getLinearVelocity();
	_vel.setX(vel[0]);
	_vel.setZ(vel[2]);
	setLinearVelocity(_vel);
}

const btVector3& CharacterController::getLinearVelocity(){
	return m_rigidBody->getLinearVelocity();
}

const float CharacterController::getLinearVelocityY() {
	return m_rigidBody->getLinearVelocity()[1];
}

void CharacterController::moveCharacterAlongY(float step){
	btVector3 pos = m_rigidBody->getWorldTransform().getOrigin();
	m_rigidBody->getWorldTransform().setOrigin(pos + btVector3(0, step, 0));
}

void CharacterController::setAngularFactor(const btVector3& angularFactor) {
	m_rigidBody->setAngularFactor(angularFactor);
}

void CharacterController::setSleepingThresholds(float linear, float angular) {
	m_rigidBody->setSleepingThresholds(linear, angular);
}

void CharacterController::setRollingFriction(float rollingFriction) {
	m_rigidBody->setRollingFriction(rollingFriction);
}

void CharacterController::setDamping(float linear, float angular) {
	m_rigidBody->setDamping(linear, angular);
}

void CharacterController::getWorldTransform(btTransform& transform) {
	m_rigidBody->getMotionState()->getWorldTransform(transform);
}

void CharacterController::setLinearFactor(const btVector3& linearFactor) {
	m_rigidBody->setLinearFactor(linearFactor);
}

void CharacterController::setGravity(const btVector3& gravity) {
	m_rigidBody->setGravity(gravity);
}

btRigidBody* CharacterController::getRigidBody() const {
	return m_rigidBody; 
}

btCollisionShape* CharacterController::getCollisionShape() const {
	return m_shape; 
}

void CharacterController::setPosition(const btVector3& position) {
	btVector3 pos = m_rigidBody->getWorldTransform().getOrigin();
	m_rigidBody->getWorldTransform().setOrigin(position);
}

void CharacterController::resetOrientation() {
	btTransform&  transform = m_rigidBody->getWorldTransform();
	transform.setRotation(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f));
}