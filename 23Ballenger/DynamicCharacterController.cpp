#include <iostream>

#include "DynamicCharacterController.h"
#include "engine/Vector.h"



const uint32_t BULLET_OBJECT_ALIGNMENT = 16;

const float DEFAULT_STEP_HEIGHT = 0.1f; // The default amount to move the character up before resolving collisions.
const btVector3 UP_VECTOR(0.0f, 1.0f, 0.0f);
const btVector3 ZERO_VECTOR(0.0f, 0.0f, 0.0f);

// Ray/convex result callbacks that ignore the self object passed to them as well as ghost objects.
class RayResultCallback : public btCollisionWorld::ClosestRayResultCallback{

public:
	RayResultCallback(btCollisionObject* self) : btCollisionWorld::ClosestRayResultCallback(ZERO_VECTOR, ZERO_VECTOR), mSelf(self){
	}

	btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace){
		if (rayResult.m_collisionObject == mSelf) return 1.0f;
		if (rayResult.m_collisionObject->getInternalType() == btCollisionObject::CO_GHOST_OBJECT) return 1.0f;
		return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
	}

private:
	btCollisionObject* mSelf;
};

class ConvexResultCallback : public btCollisionWorld::ClosestConvexResultCallback{

public:
	ConvexResultCallback(btCollisionObject* self, const btVector3& up, btScalar minSlopeDot) : btCollisionWorld::ClosestConvexResultCallback(ZERO_VECTOR, ZERO_VECTOR), mSelf(self), mUp(up), mMinSlopeDot(minSlopeDot){
	}

	btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace){
		if (convexResult.m_hitCollisionObject == mSelf) return 1.0f;
		if (convexResult.m_hitCollisionObject->getInternalType() == btCollisionObject::CO_GHOST_OBJECT) return 1.0f;

		btVector3 hitNormalWorld;
		if (normalInWorldSpace){

			hitNormalWorld = convexResult.m_hitNormalLocal;
		}
		else{

			hitNormalWorld = convexResult.m_hitCollisionObject->getWorldTransform().getBasis() * convexResult.m_hitNormalLocal;
		}

	#if 0
		btScalar dotUp = mUp.dot(hitNormalWorld);
		if (dotUp < mMinSlopeDot)
		{
			return 1.0f;
		}
	#endif
		return ClosestConvexResultCallback::addSingleResult(convexResult, normalInWorldSpace);
	}

private:
	btCollisionObject* mSelf;
	btVector3 mUp;
	btScalar mMinSlopeDot;
};

DynamicCharacterController::DynamicCharacterController()
	: mCollisionWorld(nullptr)
	, mRigidBody(nullptr)
	, mShape(nullptr)
	, mOnGround(false)
	, mOnSteepSlope(false)
	, mStepHeight(DEFAULT_STEP_HEIGHT)
	, mMaxClimbSlopeAngle(0.6f)
	, mDistanceOffset(0.1f)
	, mIsStepping(false)
{

	mSlopeNormal.setZero();

	mCharacterMovementX = 0.0f;
	mCharacterMovementZ = 0.0f;
}

DynamicCharacterController::~DynamicCharacterController(){
	
}

void DynamicCharacterController::create(btMotionState* motionState, btDynamicsWorld* physicsWorld, float mass, float radius, float height, int collisionFilterGroup, int collisionFilterMask, void* rigidBodyUserPointer) {
	
	mCollisionWorld = physicsWorld;

	// From btCapsuleShape.h:
	// "The total height is height+2*radius, so the height is just the height between the center of each 'sphere' of the capsule caps."
	float capsuleHeight = height - 2.0f * radius;

	mShape = new btCapsuleShape(capsuleHeight, radius);

	btVector3 inertia(0.0f, 0.0f, 0.0f);
	mShape->calculateLocalInertia(mass, inertia);

	btRigidBody::btRigidBodyConstructionInfo ci(mass, motionState, mShape, inertia);
	mRigidBody = new btRigidBody(ci);

	mRigidBody->setActivationState(DISABLE_DEACTIVATION);
	mRigidBody->setUserPointer(rigidBodyUserPointer);

	mRigidBody->setAngularFactor(ZERO_VECTOR); // No rotation allowed. We only rotate the visuals.
	mRigidBody->setSleepingThresholds(0.0, 0.0);
	physicsWorld->addRigidBody(mRigidBody, collisionFilterGroup, collisionFilterMask);


	mCollisionFilterGroup = collisionFilterGroup;
	mCollisionFilterMask = collisionFilterMask;
}

void DynamicCharacterController::create(btRigidBody* rigidBody, btDynamicsWorld* physicsWorld, int collisionFilterGroup, int collisionFilterMask, void* rigidBodyUserPointer) {
	mCollisionWorld = physicsWorld;
	mShape = rigidBody->getCollisionShape();
	mRigidBody = rigidBody;

	mRigidBody->setActivationState(DISABLE_DEACTIVATION);
	mRigidBody->setUserPointer(rigidBodyUserPointer);

	//mRigidBody->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
	mRigidBody->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
	//mRigidBody->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
	

	mRigidBody->setAngularFactor(ZERO_VECTOR); // No rotation allowed. We only rotate the visuals.
	mRigidBody->setSleepingThresholds(0.0, 0.0);
	physicsWorld->addRigidBody(mRigidBody, collisionFilterGroup, collisionFilterMask);

	mCollisionFilterGroup = collisionFilterGroup;
	mCollisionFilterMask = collisionFilterMask;
}

void DynamicCharacterController::destroy(){
	delete mRigidBody;
	mRigidBody = nullptr;

	delete mShape;
	mShape = nullptr;

	mCollisionWorld = nullptr;
}

void DynamicCharacterController::setParameters(float maxClimbSlopeAngle){
	mMaxClimbSlopeAngle = maxClimbSlopeAngle;
}

void DynamicCharacterController::setSlopeAngle(float degrees) {
	mMaxClimbSlopeAngle = cos(degrees * PI * (1.0f / 180.0f));
}

void DynamicCharacterController::setDistanceOffset(float value) {
	mDistanceOffset = value;
}

void DynamicCharacterController::preStep(){
	
	mIsStepping = true;

	if (mOnSteepSlope){
		btVector3 uAxis = mSlopeNormal.cross(UP_VECTOR).normalize();
		btVector3 vAxis = uAxis.cross(mSlopeNormal);
		btVector3 fixVel = vAxis / mSlopeNormal.dot(UP_VECTOR);
		mRigidBody->setLinearVelocity(mRigidBody->getLinearVelocity() - fixVel);
	}

	// Move character upwards. (Bump over stairs)
	if (movingUpward) {
		moveCharacterAlongY(mStepHeight);
	}
	
	// Set the linear velocity based on the movement vector. Don't adjust the Y-component, instead let Bullet handle that.
	if (mOnGround){		
		btVector3 linVel = mRigidBody->getLinearVelocity();
		linVel.setX(mCharacterMovementX);
		linVel.setZ(mCharacterMovementZ);
		mRigidBody->setLinearVelocity(linVel);
	}
}

// If defined, a ray test is used to detect objects below the character, otherwise a convex test.
#define USE_RAY_TEST 

void DynamicCharacterController::postStep() {
	const float TEST_DISTANCE = 2.0;
	const float TEST_DISTANCE_RAY = 7.0;

#ifdef USE_RAY_TEST
	btVector3 from = mRigidBody->getWorldTransform().getOrigin();
	btVector3 to = from - btVector3(0, TEST_DISTANCE_RAY, 0);

	// Detect ground collision and update the "on ground" status.
	RayResultCallback callback(mRigidBody);
	callback.m_collisionFilterGroup = mRigidBody->getBroadphaseHandle()->m_collisionFilterGroup;
	callback.m_collisionFilterMask = mRigidBody->getBroadphaseHandle()->m_collisionFilterMask;
	mCollisionWorld->rayTest(from, to, callback);

	// Check if there is something below the character.
	if (callback.hasHit()){

		btVector3 end = from + (to + btVector3(0.0, TEST_DISTANCE_RAY, 0.0) - from) * callback.m_closestHitFraction;
		btVector3 normal = callback.m_hitNormalWorld;

		// Slope test.
		btScalar slopeDot = normal.dot(UP_VECTOR);
		mOnSteepSlope = (slopeDot < mMaxClimbSlopeAngle);
		mSlopeNormal = normal;
		
		// compute the distance to the floor
		float distance = btDistance(end, from);
		mOnGround = (distance < 1.0 && !mOnSteepSlope);		
		// Move down.
		if (distance < mStepHeight) {
			moveCharacterAlongY(-distance * 0.99999f);
		}else{
			moveCharacterAlongY(-mStepHeight * 0.9999f);
		}

	}else{
		// In the air.
		mOnGround = false;
		moveCharacterAlongY(-mStepHeight);
	}
#else
	btTransform tsFrom, tsTo;
	btVector3 from = mRigidBody->getWorldTransform().getOrigin();
	btVector3 to = from - btVector3(0, TEST_DISTANCE, 0);

	tsFrom.setIdentity();
	tsFrom.setOrigin(from);

	tsTo.setIdentity();
	tsTo.setOrigin(to);

	ConvexResultCallback callback(mRigidBody, UP_VECTOR, 0.01f);
	callback.m_collisionFilterGroup = mRigidBody->getBroadphaseHandle()->m_collisionFilterGroup;
	callback.m_collisionFilterMask = mRigidBody->getBroadphaseHandle()->m_collisionFilterMask;
	mCollisionWorld->convexSweepTest((btConvexShape*)mShape, tsFrom, tsTo, callback, mCollisionWorld->getDispatchInfo().m_allowedCcdPenetration);
	
	if (callback.hasHit()){

		btVector3 end = from + (to - from) * callback.m_closestHitFraction;
		btVector3 normal = callback.m_hitNormalWorld;
		
		// Slope test.
		btScalar slopeDot = normal.dot(btVector3(0, 1, 0));
		mOnSteepSlope = (slopeDot < mMaxClimbSlopeAngle);
		mSlopeNormal = normal;
		
		// Compute distance to the floor.
		float distance = btDistance(end, from);
		mOnGround = (distance < mDistanceOffset && !mOnSteepSlope);

		// Move down.
		if (distance < mStepHeight){
			
			moveCharacterAlongY(-distance * 0.99999f);
		}else {
			moveCharacterAlongY(-mStepHeight * 0.9999f);
		}
	} else{

		mOnGround = false;
		moveCharacterAlongY(-mStepHeight);
	}
#endif

	mIsStepping = false;
}

bool DynamicCharacterController::isStepping() const{

	return mIsStepping;
}

bool DynamicCharacterController::onGround() const{

	return mOnGround;
}

void DynamicCharacterController::jump(const btVector3& direction, float force){
	
	if (mOnGround){		
	
		mOnGround = false;
		mRigidBody->applyCentralImpulse(direction * force);
	}
}

void DynamicCharacterController::setMovementXZ(const Vector2f& movementVector){
	mCharacterMovementX = movementVector[0];
	mCharacterMovementZ = movementVector[1];
	//mCharacterMovementY = 0;
}

void  DynamicCharacterController::setMovementXYZ(const btVector3& movementVector){
	mCharacterMovementX = movementVector[0];
	mCharacterMovementY = movementVector[1];
	mCharacterMovementZ = movementVector[2];
}

void DynamicCharacterController::setLinearVelocity(const btVector3& vel){	
	mRigidBody->setLinearVelocity(vel);
}

const btVector3& DynamicCharacterController::getLinearVelocity(){
	return mRigidBody->getLinearVelocity();
}

void DynamicCharacterController::moveCharacterAlongY(float step){
	btVector3 pos = mRigidBody->getWorldTransform().getOrigin();
	mRigidBody->getWorldTransform().setOrigin(pos + btVector3(0, step, 0));
}

void DynamicCharacterController::setStepHeight(float value) {
	mStepHeight = value;
}