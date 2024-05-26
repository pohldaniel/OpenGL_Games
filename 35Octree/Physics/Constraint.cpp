#include "Constraint.h"

static const char* typeNames[] ={
	"Point",
	"Hinge",
	"Slider",
	"ConeTwist",
	0
};

Constraint::Constraint() :
	constraintType_(CONSTRAINT_POINT),
	position_(Vector3f::ZERO),
	rotation_(Quaternion::IDENTITY),
	otherPosition_(Vector3f::ZERO),
	otherRotation_(Quaternion::IDENTITY),
	highLimit_(Vector2f(0.0f, 0.0f)),
	lowLimit_(Vector2f(0.0f, 0.0f)),
	erp_(0.0f),
	cfm_(0.0f),
	otherBodyNodeID_(0),
	disableCollision_(false),
	recreateConstraint_(true),
	framesDirty_(false),
	retryCreation_(false)
{
}

Constraint::~Constraint(){
	ReleaseConstraint();

	if (physicsWorld_)
		physicsWorld_->removeConstraint(constraint_);
}

void Constraint::OnSetEnabled(){
	if (constraint_)
		constraint_->setEnabled(true);
}

/*void Constraint::GetDependencyNodes(PODVector<Node*>& dest)
{
	if (otherBody_ && otherBody_->GetNode())
		dest.Push(otherBody_->GetNode());
}*/

void Constraint::SetConstraintType(ConstraintType type)
{
	if (type != constraintType_)
	{
		constraintType_ = type;
		CreateConstraint();
		//MarkNetworkUpdate();
	}
}

void Constraint::SetOtherBody(btRigidBody* body){
	if (otherBody_ != body){
		//if (otherBody_)
			//otherBody_->removeConstraint(this);

		otherBody_ = body;

		// Update the connected body attribute
		//Node* otherNode = otherBody_ ? otherBody_->GetNode() : 0;
		//otherBodyNodeID_ = otherNode ? otherNode->GetID() : 0;

		CreateConstraint();
		//MarkNetworkUpdate();
	}
}

void Constraint::SetPosition(const Vector3f& position){
	if (position != position_){
		position_ = position;
		ApplyFrames();
		//MarkNetworkUpdate();
	}
}

void Constraint::SetRotation(const Quaternion& rotation){
	if (rotation != rotation_){
		rotation_ = rotation;
		ApplyFrames();
		//MarkNetworkUpdate();
	}
}

void Constraint::SetAxis(const Vector3f& axis)
{
	switch (constraintType_)
	{
	case CONSTRAINT_POINT:
	case CONSTRAINT_HINGE:
		rotation_ = Quaternion(Vector3f::FORWARD, axis);
		break;

	case CONSTRAINT_SLIDER:
	case CONSTRAINT_CONETWIST:
		rotation_ = Quaternion(Vector3f::RIGHT, axis);
		break;

	default:
		break;
	}

	ApplyFrames();
	//MarkNetworkUpdate();
}

void Constraint::SetOtherPosition(const Vector3f& position){
	if (position != otherPosition_){
		otherPosition_ = position;
		ApplyFrames();
		//MarkNetworkUpdate();
	}
}

void Constraint::SetOtherRotation(const Quaternion& rotation){
	if (rotation != otherRotation_){
		otherRotation_ = rotation;
		ApplyFrames();
		//MarkNetworkUpdate();
	}
}

void Constraint::SetOtherAxis(const Vector3f& axis){
	switch (constraintType_){
	case CONSTRAINT_POINT:
	case CONSTRAINT_HINGE:
		otherRotation_ = Quaternion(Vector3f::FORWARD, axis);
		break;

	case CONSTRAINT_SLIDER:
	case CONSTRAINT_CONETWIST:
		otherRotation_ = Quaternion(Vector3f::RIGHT, axis);
		break;

	default:
		break;
	}

	ApplyFrames();
	//MarkNetworkUpdate();
}

void Constraint::SetWorldPosition(const Vector3f& position){
	if (constraint_){
		btTransform ownBodyInverse = constraint_->getRigidBodyA().getWorldTransform().inverse();
		btTransform otherBodyInverse = constraint_->getRigidBodyB().getWorldTransform().inverse();
		btVector3 worldPos = Physics::VectorFrom(position);
		position_ = (Physics::VectorFrom(ownBodyInverse * worldPos) + Vector3f::ZERO) / cachedWorldScale_;
		otherPosition_ = Physics::VectorFrom(otherBodyInverse * worldPos);
		if (otherBody_){
			//otherPosition_ += otherBody_->GetCenterOfMass();
			//otherPosition_ /= otherBody_->GetNode()->GetWorldScale();
		}
		ApplyFrames();
		//MarkNetworkUpdate();
	}
}

void Constraint::SetHighLimit(const Vector2f& limit){
	if (limit != highLimit_){
		highLimit_ = limit;
		ApplyLimits();
		//MarkNetworkUpdate();
	}
}

void Constraint::SetLowLimit(const Vector2f& limit){
	if (limit != lowLimit_){
		lowLimit_ = limit;
		ApplyLimits();
		//MarkNetworkUpdate();
	}
}

void Constraint::SetERP(float erp){
	erp = std::max(erp, 0.0f);

	if (erp != erp_){
		erp_ = erp;
		ApplyLimits();
		//MarkNetworkUpdate();
	}
}

void Constraint::SetCFM(float cfm){
	cfm = std::max(cfm, 0.0f);

	if (cfm != cfm_){
		cfm_ = cfm;
		ApplyLimits();
		//MarkNetworkUpdate();
	}
}

void Constraint::SetDisableCollision(bool disable){
	if (disable != disableCollision_){
		disableCollision_ = disable;
		CreateConstraint();
		//MarkNetworkUpdate();
	}
}

Vector3f Constraint::GetWorldPosition() const{
	if (constraint_){
		btTransform ownBody = constraint_->getRigidBodyA().getWorldTransform();
		return Physics::VectorFrom(ownBody * Physics::VectorFrom(position_ * cachedWorldScale_ - Vector3f::ZERO));
	}else
		return Vector3f::ZERO;
}

void Constraint::ReleaseConstraint(){
	if (constraint_){
		//if (ownBody_)
		//	ownBody_->RemoveConstraint(this);
		//if (otherBody_)
		//	otherBody_->RemoveConstraint(this);

		if (physicsWorld_)
			physicsWorld_->removeConstraint(constraint_);

		//constraint_.reset();
		delete constraint_;
	}
}

void Constraint::CreateConstraint(){
	

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
	//btRigidBody* otherBody = &btTypedConstraint::getFixedBody();

	Vector3f ownBodyScaledPosition = position_ * cachedWorldScale_ - Vector3f::ZERO;
	Vector3f otherBodyScaledPosition = otherBody_ ? otherPosition_  - Vector3f::ZERO : otherPosition_;

	switch (constraintType_)
	{
	case CONSTRAINT_POINT:
	{
		constraint_ = new btPoint2PointConstraint(*ownBody_, *otherBody_, Physics::VectorFrom(ownBodyScaledPosition),
			Physics::VectorFrom(otherBodyScaledPosition));
	}
	break;

	case CONSTRAINT_HINGE:
	{
		btTransform ownFrame(Physics::QuaternionFrom(rotation_), Physics::VectorFrom(ownBodyScaledPosition));
		btTransform otherFrame(Physics::QuaternionFrom(otherRotation_), Physics::VectorFrom(otherBodyScaledPosition));
		constraint_ = new btHingeConstraint(*ownBody_, *otherBody_, ownFrame, otherFrame);
	}
	break;

	case CONSTRAINT_SLIDER:
	{
		btTransform ownFrame(Physics::QuaternionFrom(rotation_), Physics::VectorFrom(ownBodyScaledPosition));
		btTransform otherFrame(Physics::QuaternionFrom(otherRotation_), Physics::VectorFrom(otherBodyScaledPosition));
		constraint_ = new btSliderConstraint(*ownBody_, *otherBody_, ownFrame, otherFrame, false);
	}
	break;

	case CONSTRAINT_CONETWIST:
	{
		btTransform ownFrame(Physics::QuaternionFrom(rotation_), Physics::VectorFrom(ownBodyScaledPosition));
		btTransform otherFrame(Physics::QuaternionFrom(otherRotation_), Physics::VectorFrom(otherBodyScaledPosition));
		constraint_ = new btConeTwistConstraint(*ownBody_, *otherBody_, ownFrame, otherFrame);
	}
	break;

	default:
		break;
	}

	if (constraint_){
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

void Constraint::ApplyLimits()
{
	if (!constraint_)
		return;

	switch (constraint_->getConstraintType())
	{
	case HINGE_CONSTRAINT_TYPE:
	{
		btHingeConstraint* hingeConstraint = static_cast<btHingeConstraint*>(constraint_);
		hingeConstraint->setLimit(lowLimit_[0] * PI_ON_180, highLimit_[0] * PI_ON_180);
	}
	break;

	case SLIDER_CONSTRAINT_TYPE:
	{
		btSliderConstraint* sliderConstraint = static_cast<btSliderConstraint*>(constraint_);
		sliderConstraint->setUpperLinLimit(highLimit_[0]);
		sliderConstraint->setUpperAngLimit(highLimit_[1] * PI_ON_180);
		sliderConstraint->setLowerLinLimit(lowLimit_[0]);
		sliderConstraint->setLowerAngLimit(lowLimit_[1] * PI_ON_180);
	}
	break;

	case CONETWIST_CONSTRAINT_TYPE:
	{
		btConeTwistConstraint* coneTwistConstraint = static_cast<btConeTwistConstraint*>(constraint_);
		coneTwistConstraint->setLimit(highLimit_[1] * PI_ON_180, highLimit_[1] * PI_ON_180, highLimit_[0] * PI_ON_180);
	}
	break;

	default:
		break;
	}

	if (erp_ != 0.0f)
		constraint_->setParam(BT_CONSTRAINT_STOP_ERP, erp_);
	if (cfm_ != 0.0f)
		constraint_->setParam(BT_CONSTRAINT_STOP_CFM, cfm_);
}

void Constraint::ApplyFrames(){
	//if (!constraint_ || !node_ || (otherBody_ && !otherBody_->GetNode()))
		//return;

	cachedWorldScale_ = Vector3f::ONE;

	Vector3f ownBodyScaledPosition = position_ * cachedWorldScale_;
	Vector3f otherBodyScaledPosition =  otherPosition_;

	switch (constraint_->getConstraintType()){
	case POINT2POINT_CONSTRAINT_TYPE:{
		btPoint2PointConstraint* pointConstraint = static_cast<btPoint2PointConstraint*>(constraint_);
		pointConstraint->setPivotA(Physics::VectorFrom(ownBodyScaledPosition));
		pointConstraint->setPivotB(Physics::VectorFrom(otherBodyScaledPosition));
	}
	break;

	case HINGE_CONSTRAINT_TYPE:
	{
		btHingeConstraint* hingeConstraint = static_cast<btHingeConstraint*>(constraint_);
		btTransform ownFrame(Physics::QuaternionFrom(rotation_), Physics::VectorFrom(ownBodyScaledPosition));
		btTransform otherFrame(Physics::QuaternionFrom(otherRotation_), Physics::VectorFrom(otherBodyScaledPosition));
		hingeConstraint->setFrames(ownFrame, otherFrame);
	}
	break;

	case SLIDER_CONSTRAINT_TYPE:
	{
		btSliderConstraint* sliderConstraint = static_cast<btSliderConstraint*>(constraint_);
		btTransform ownFrame(Physics::QuaternionFrom(rotation_), Physics::VectorFrom(ownBodyScaledPosition));
		btTransform otherFrame(Physics::QuaternionFrom(otherRotation_), Physics::VectorFrom(otherBodyScaledPosition));
		sliderConstraint->setFrames(ownFrame, otherFrame);
	}
	break;

	case CONETWIST_CONSTRAINT_TYPE:
	{
		btConeTwistConstraint* coneTwistConstraint = static_cast<btConeTwistConstraint*>(constraint_);
		btTransform ownFrame(Physics::QuaternionFrom(rotation_), Physics::VectorFrom(ownBodyScaledPosition));
		btTransform otherFrame(Physics::QuaternionFrom(otherRotation_), Physics::VectorFrom(otherBodyScaledPosition));
		coneTwistConstraint->setFrames(ownFrame, otherFrame);
	}
	break;

	default:
		break;
	}
}