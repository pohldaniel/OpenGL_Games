#include "Constraint6DoF.h"

static const char* typeNames[] =
{
	"Generic6DoFSpring2",
	0
};
const Vector3f DefaultERP(0.2f, 0.2f, 0.2f);

Constraint6DoF::Constraint6DoF()
	: linearLowerLimit_(Vector3f::ONE)
	, linearUpperLimit_(Vector3f::ZERO)
	, angularLowerLimit_(Vector3f::ONE)
	, angularUpperLimit_(Vector3f::ZERO)

	, linearServoEnable_({0,0,0})
	, angularServoEnable_({ 0,0,0 })

	, linearMotorEnable_({ 0,0,0 })
	, angularMotorEnable_({ 0,0,0 })
	, linearMotorForce_(Vector3f::ZERO)
	, angularMotorForce_(Vector3f::ZERO)

	, linearBounceForce_(Vector3f::ZERO)
	, angularBounceForce_(Vector3f::ZERO)

	, linearSpringEnable_({ 0,0,0 })
	, angularSpringEnable_({ 0,0,0 })

	, linearStiffness_(Vector3f::ZERO)
	, angularStiffness_(Vector3f::ZERO)
	, linearStiffnessLimit_({1,1,1})
	, angularStiffnessLimit_({ 1,1,1 })

	, linearDamping_(Vector3f::ZERO)
	, angularDamping_(Vector3f::ZERO)
	, linearDampingLimit_({ 1,1,1 })
	, angularDampingLimit_({ 1,1,1 })

	, linearErps_(DefaultERP)
	, angularErps_(DefaultERP)
	, linearCfms_(Vector3f::ZERO)
	, angularCfms_(Vector3f::ZERO)

	, linearStopErps_(DefaultERP)
	, angularStopErps_(DefaultERP)
	, linearStopCfms_(Vector3f::ZERO)
	, angularStopCfms_(Vector3f::ZERO){
	constraintType_ = CONSTRAINT_GENERIC6DOFSPRING2;
}

Constraint6DoF::~Constraint6DoF(){
	ReleaseConstraint();

	if (physicsWorld_)
		physicsWorld_->removeConstraint(constraint_);
}

void Constraint6DoF::ApplyFrames(){
	
}

void Constraint6DoF::CreateConstraint(){


	cachedWorldScale_ = Vector3f::ONE;

	ReleaseConstraint();

	//ownBody_ = GetComponent<RigidBody>();
	//btRigidBody* ownBody = ownBody_ ? ownBody_->GetBody() : 0;
	//btRigidBody* otherBody = otherBody_ ? otherBody_->GetBody() : 0;

	// If no physics world available now mark for retry later
	//if (!physicsWorld_ || !ownBody)
	//{
	//	retryCreation_ = true;
	//	return;
	//}

	//if (!otherBody)
	//{
	//	otherBody = &btTypedConstraint::getFixedBody();
	//}

	Vector3f ownBodyScaledPosition = position_ * cachedWorldScale_;
	Vector3f otherBodyScaledPosition = otherBody_ ? otherPosition_ - Vector3f::ZERO : otherPosition_;

	btTransform ownFrame(Physics::QuaternionFrom(rotation_), Physics::VectorFrom(ownBodyScaledPosition));
	btTransform otherFrame(Physics::QuaternionFrom(otherRotation_), Physics::VectorFrom(otherBodyScaledPosition));
	btGeneric6DofSpring2Constraint *gen6dofConstraint = new btGeneric6DofSpring2Constraint(*ownBody_, *otherBody_, ownFrame, otherFrame);
	gen6dofConstraint->setDbgDrawSize(0.5f);
	constraint_ = gen6dofConstraint;

	if (constraint_)
	{
		constraint_->setUserConstraintPtr(this);
		constraint_->setEnabled(true);
		//ownBody_->AddConstraint(this);
		//if (otherBody_)
			//otherBody_->AddConstraint(this);

		ApplyLimits();

		physicsWorld_->addConstraint(constraint_, disableCollision_);
	}

	recreateConstraint_ = false;
	framesDirty_ = false;
	retryCreation_ = false;
}

void Constraint6DoF::ApplyLimits()
{
	if (!constraint_)
		return;

	// push settings to Bullet
	Constraint::ApplyLimits();
	/*SetLimits();
	SetServo();
	SetEnableMotor();
	SetMaxMotorForce();
	SetBounce();
	SetEnableSpring();
	SetStiffness();
	SetDamping();
	SetERPs();
	SetCFMs();
	SetStopERPs();
	SetStopCFMs();*/
}