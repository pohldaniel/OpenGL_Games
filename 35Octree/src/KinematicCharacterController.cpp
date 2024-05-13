#include "KinematicCharacterController.h"

//=============================================================================
//=============================================================================
const float STEP_HEIGHT = 0.65f;
const float JUMP_HEIGHT = 4.0f;
const float FALL_SPEED = 55.0f;
const float JUMP_SPEED = 12.0f;
const float MAX_SLOPE = 45.0f;
const float DEFAULT_DAMPING = 0.2f;
const Vector3f KINEMATIC_GRAVITY(0.0f, -14.0f, 0.0f);

//=============================================================================
//=============================================================================
btPairCachingGhostObject* newPairCachingGhostObj(){
	return new btPairCachingGhostObject();
}

btKinematicCharacterController* newKinematicCharCtrl(btPairCachingGhostObject *ghostCGO, btConvexShape *shape, float stepHeight, const btVector3 &upVec){
	return new btKinematicCharacterController(ghostCGO, shape, stepHeight, upVec);
}


//=============================================================================
//=============================================================================
KinematicCharacterController::KinematicCharacterController()
	: colFilter_(Physics::collisiontypes::CHARACTER)
	, colMask_(Physics::collisiontypes::FLOOR)
	, gravity_(KINEMATIC_GRAVITY)
	, stepHeight_(STEP_HEIGHT)
	, maxJumpHeight_(JUMP_HEIGHT)
	, fallSpeed_(FALL_SPEED)
	, jumpSpeed_(JUMP_SPEED)
	, maxSlope_(MAX_SLOPE)
	, linearDamping_(DEFAULT_DAMPING)
	, angularDamping_(DEFAULT_DAMPING)
	, colShapeOffset_(Vector3f::ZERO)
	, reapplyAttributes_(false)
{
	pairCachingGhostObject_.reset(newPairCachingGhostObj());
	pairCachingGhostObject_->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	position_ = Vector3f(28.0f, 8.84f, -4.0f);
	rotation_ = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

	physicsWorld_ = Physics::GetDynamicsWorld();

	m_ghostPairCallback = new btGhostPairCallback();
	Physics::GetDynamicsWorld()->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(m_ghostPairCallback);

	AddKinematicToWorld();
}

KinematicCharacterController::~KinematicCharacterController() {
	ReleaseKinematic();

	// Delete GhostPair callback
	if (m_ghostPairCallback) {
		delete m_ghostPairCallback;
		m_ghostPairCallback = 0;
	}
}

void KinematicCharacterController::AddKinematicToWorld()
{
	if (physicsWorld_)
	{
		if (!kinematicController_)
		{
			btCapsuleShape* colShape = new btCapsuleShape(0.7f * 0.5f, std::max(1.8f - 0.7f, 0.0f));

			pairCachingGhostObject_->setCollisionShape(colShape);
			colShapeOffset_ = Vector3f(0.0f, 0.84f, 0.0f);
			kinematicController_.reset(newKinematicCharCtrl(pairCachingGhostObject_.get(), colShape, stepHeight_, btVector3(0.0f, 1.0f, 0.0f)));
			// apply default settings
			ApplySettings();

			btDiscreteDynamicsWorld *phyicsWorld = Physics::GetDynamicsWorld();
			phyicsWorld->addCollisionObject(pairCachingGhostObject_.get(), colFilter_, colMask_);
			phyicsWorld->addAction(kinematicController_.get());
		}
	}
}

void KinematicCharacterController::ApplySettings(bool reapply)
{
	kinematicController_->setGravity(Physics::VectorFrom(gravity_));
	kinematicController_->setLinearDamping(linearDamping_);
	kinematicController_->setAngularDamping(angularDamping_);
	kinematicController_->setStepHeight(stepHeight_);
	kinematicController_->setMaxJumpHeight(maxJumpHeight_);
	kinematicController_->setMaxSlope(PI_ON_180 * maxSlope_);
	kinematicController_->setJumpSpeed(jumpSpeed_);
	kinematicController_->setFallSpeed(fallSpeed_);

	if (reapply && pairCachingGhostObject_)
	{
		btDiscreteDynamicsWorld *phyicsWorld = Physics::GetDynamicsWorld();
		phyicsWorld->removeCollisionObject(pairCachingGhostObject_.get());
		phyicsWorld->addCollisionObject(pairCachingGhostObject_.get(), colFilter_, colMask_);
	}

	SetTransform(position_, rotation_);
}

void KinematicCharacterController::ReleaseKinematic()
{
	if (kinematicController_)
	{
		RemoveKinematicFromWorld();
	}

	kinematicController_.reset();
	pairCachingGhostObject_.reset();
}

void KinematicCharacterController::RemoveKinematicFromWorld()
{
	if (kinematicController_ && physicsWorld_)
	{
		physicsWorld_->removeCollisionObject(pairCachingGhostObject_.get());
		physicsWorld_->removeAction(kinematicController_.get());
	}
}

void KinematicCharacterController::SetTransform(const Vector3f& position, const Quaternion& rotation)
{
	btTransform worldTrans;
	worldTrans.setIdentity();
	worldTrans.setRotation(Physics::QuaternionFrom(rotation));
	worldTrans.setOrigin(Physics::VectorFrom(position));
	pairCachingGhostObject_->setWorldTransform(worldTrans);
}

const Vector3f& KinematicCharacterController::GetPosition() {
	btTransform t = pairCachingGhostObject_->getWorldTransform();
	position_ = Physics::VectorFrom(t.getOrigin()) - colShapeOffset_;
	return position_;
}

const Quaternion& KinematicCharacterController::GetRotation() {
	btTransform t = pairCachingGhostObject_->getWorldTransform();
	rotation_ = Physics::QuaternionFrom(t.getRotation());
	return rotation_;
}

btTransform& KinematicCharacterController::GetTransform() {
	return pairCachingGhostObject_->getWorldTransform();
}

void KinematicCharacterController::GetTransform(Vector3f& position, Quaternion& rotation) {
	btTransform worldTrans = pairCachingGhostObject_->getWorldTransform();
	rotation = Physics::QuaternionFrom(worldTrans.getRotation());
	position = Physics::VectorFrom(worldTrans.getOrigin());
}

void KinematicCharacterController::moveAlongY(float step) {
	btVector3 pos = pairCachingGhostObject_->getWorldTransform().getOrigin();
	pairCachingGhostObject_->getWorldTransform().setOrigin(pos + btVector3(0, step, 0));
}

bool KinematicCharacterController::OnGround() const {
	return kinematicController_->onGround();
}

void KinematicCharacterController::SetWalkDirection(const Vector3f& walkDir) {
	kinematicController_->setWalkDirection(Physics::VectorFrom(walkDir));
}

void KinematicCharacterController::Jump(const Vector3f&jump) {
	kinematicController_->jump(Physics::VectorFrom(jump));
}

void KinematicCharacterController::DebugDrawContacts() {

	btManifoldArray	manifoldArray;
	btBroadphasePairArray& pairArray = pairCachingGhostObject_->getOverlappingPairCache()->getOverlappingPairArray();
	int numPairs = pairArray.size();

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
	pairCachingGhostObject_.get()->setUserPointer(userPointer);
}

void KinematicCharacterController::setPosition(const Vector3f& position) {
	position_ = position;
	SetTransform(position_, rotation_);
}