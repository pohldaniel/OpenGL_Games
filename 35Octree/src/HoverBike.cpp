#include <engine/input/Keyboard.h>
#include "HoverBike.h"

const Vector3f wheelDirectionCS0(0, -1, 0);
const Vector3f wheelAxleCS(-1, 0, 0);
const int rightIndex = 0;
const int upIndex = 1;
const int forwardIndex = 2;

HoverBike::HoverBike(const Shape& shape, const Camera& camera) : ShapeEntity(shape, camera),
raycastVehicle_(NULL),
vehicleRaycaster_(NULL),
engineForce_(10.0f),
maxSpeed_(150.0f),
minBrakeForce_(2.0f),
maxBrakeForce_(300.0f),
wheelRadius_(0.9f),
wheelFriction_(0.9f),

steeringIncrement_(0.02f),
steeringClamp_(0.4f),
maxRollAngle_(30.0f),
maxSpeedToMaxRollAngle_(120.0f),

suspensionRestLength_(0.7f),
suspensionStiffness_(25.0f),
suspensionRelaxation_(7.0f),
suspensionCompression_(10.0f),
rollInfluence_(0.3f),

currentSteering_(0.0f),
softPitchLimit_(40.0f),
m_bikeBody(nullptr),
m_cubeBody(nullptr){

	m_bikeBody = Physics::AddRigidBody(5.0f, Physics::BtTransform(btVector3(80.0f, 2.0f, 40.0f)), Physics::CreateConvexHullShape(&shape), Physics::PICKABLE_OBJECT, Physics::collisiontypes::FLOOR | Physics::MOUSEPICKER);
	//m_cubeBody = Physics::AddKinematicRigidBody(Physics::BtTransform(btVector3(80.0f, 2.0f, 40.0f)), new btBoxShape(btVector3(0.1f, 0.1f, 0.1f)), Physics::PICKABLE_OBJECT, Physics::collisiontypes::FLOOR | Physics::MOUSEPICKER, nullptr, false);
	Create();

	pGen6DOFSpring = new btGeneric6DofConstraint(*m_bikeBody, Physics::BtTransform(btVector3(0.0f, 0.0f, 0.0f)), false);
	//pGen6DOFSpring = new btGeneric6DofSpring2Constraint(*m_cubeBody, *m_bikeBody, Physics::BtTransform(btVector3(0.0f, 0.0f, 0.0f)), Physics::BtTransform(btVector3(0.0f, 0.0f, 0.0f)));
	pGen6DOFSpring->setLinearLowerLimit(btVector3(1.0f, 1.0f, 1.0f));
	pGen6DOFSpring->setLinearUpperLimit(btVector3(0.0f, 0.0f, 0.0f));
	pGen6DOFSpring->setAngularLowerLimit(btVector3(1.0f, 0.0f, 0.0f));
	pGen6DOFSpring->setAngularUpperLimit(btVector3(0.0f, 0.0f, 0.0f));

	//pGen6DOFSpring->setParam(BT_CONSTRAINT_ERP, 0.8f, 0);
	//pGen6DOFSpring->setParam(BT_CONSTRAINT_ERP, 0.8f, 1);
	//pGen6DOFSpring->setParam(BT_CONSTRAINT_ERP, 0.8f, 2);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_ERP, 0.04f, 3);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_ERP, 0.1f, 4);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_ERP, 0.2f, 5);

	//pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_ERP, 0.2f, 0);
	//pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_ERP, 0.2f, 1);
	//pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_ERP, 0.2f, 2);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_ERP, 0.06f, 3);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_ERP, 0.05f, 4);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_ERP, 0.06f, 5);

	//pGen6DOFSpring->setParam(BT_CONSTRAINT_CFM, 0.8f, 0);
	//pGen6DOFSpring->setParam(BT_CONSTRAINT_CFM, 0.8f, 1);
	//pGen6DOFSpring->setParam(BT_CONSTRAINT_CFM, 0.8f, 2);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_CFM, 0.001f, 3);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_CFM, 0.0f, 4);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_CFM, 0.04f, 5);

	//pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_CFM, 0.2f, 0);
	//pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_CFM, 0.2f, 1);
	//pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_CFM, 0.2f, 2);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_CFM, 0.001f, 3);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_CFM, 1.0f, 4);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_CFM, 0.4f, 5);

	pGen6DOFSpring->setDbgDrawSize(btScalar(5.0f));
	Physics::GetDynamicsWorld()->addConstraint(pGen6DOFSpring, true);
}

HoverBike::~HoverBike() {
	if (vehicleRaycaster_) {
		delete vehicleRaycaster_;
		vehicleRaycaster_ = NULL;
	}

	if (raycastVehicle_) {
		btDynamicsWorld *dynamicWorld = Physics::GetDynamicsWorld();
		dynamicWorld->removeVehicle(raycastVehicle_);
		delete raycastVehicle_;
		raycastVehicle_ = NULL;
	}
}

void HoverBike::DelayedStart() {
	// create raycast vehicle
	if (raycastVehicle_ == NULL) {
		Create();
	}
}

bool HoverBike::Create() {
	CreateRaycastVehicle();
	return true;
}

void HoverBike::CreateRaycastVehicle() {
	if (raycastVehicle_) {
		return;
	}

	btDynamicsWorld *pbtDynWorld = Physics::GetDynamicsWorld();
	vehicleRaycaster_ = new btFilteredVehicleRaycaster(pbtDynWorld, Physics::PICKABLE_OBJECT, Physics::collisiontypes::FLOOR | Physics::MOUSEPICKER);
	raycastVehicle_ = new btRaycastVehicle(vehicleTuning_, m_bikeBody, vehicleRaycaster_);

	pbtDynWorld->addVehicle(raycastVehicle_);
	raycastVehicle_->setCoordinateSystem(rightIndex, upIndex, forwardIndex);

	// add wheels
	raycastVehicle_->addWheel(Physics::VectorFrom(Vector3f(0.0f, 0.05f, 1.15842f)), Physics::VectorFrom(wheelDirectionCS0), Physics::VectorFrom(wheelAxleCS), suspensionRestLength_, wheelRadius_, vehicleTuning_, true);
	raycastVehicle_->addWheel(Physics::VectorFrom(Vector3f(0.0f, 0.05f, -1.62427f)), Physics::VectorFrom(wheelDirectionCS0), Physics::VectorFrom(wheelAxleCS), suspensionRestLength_, wheelRadius_, vehicleTuning_, false);

	// config suspension
	for (int i = 0; i < raycastVehicle_->getNumWheels(); ++i) {
		btWheelInfo& wheel = raycastVehicle_->getWheelInfo(i);
		wheel.m_suspensionStiffness = suspensionStiffness_;
		wheel.m_wheelsDampingRelaxation = suspensionRelaxation_;
		wheel.m_wheelsDampingCompression = suspensionCompression_;
		wheel.m_frictionSlip = wheelFriction_;
		wheel.m_rollInfluence = rollInfluence_;
	}
}

void HoverBike::FixedUpdate(float timeStep) {
	Keyboard &keyboard = Keyboard::instance();

	float newSteering = 0.0f;
	float accelerator = 0.0f;
	bool braking = false;

	// Read controls
	if (keyboard.keyDown(Keyboard::KEY_RIGHT))
		newSteering = -1.0f;
	if (keyboard.keyDown(Keyboard::KEY_LEFT))
		newSteering = 1.0f;
	if (keyboard.keyDown(Keyboard::KEY_UP))
		accelerator = 1.0f;
	if (keyboard.keyDown(Keyboard::KEY_DOWN))
		accelerator = -0.4f;
	if (keyboard.keyDown(Keyboard::KEY_SPACE))
	{
		braking = true;
		accelerator = 0.0f;
	}

	if (newSteering != 0.0f || accelerator != 0.0f) {
		m_bikeBody->activate(true);
	}

	if (newSteering != 0.0f) {
		currentSteering_ += steeringIncrement_ * newSteering;
	}else {
		currentSteering_ *= 0.7f;
	}
	currentSteering_ = Math::Clamp(currentSteering_, -steeringClamp_, steeringClamp_);
	raycastVehicle_->setSteeringValue(currentSteering_, 0);

	float engineForce = (std::fabs(accelerator) > 0.0f && !braking) ? engineForce_ * accelerator : 0.0f;

	for (int i = 0; i < raycastVehicle_->getNumWheels(); ++i) {
		raycastVehicle_->applyEngineForce(engineForce, i);

		if (i > 0) {
			raycastVehicle_->setBrake(braking ? maxBrakeForce_ : minBrakeForce_, i);
		}
	}

	UpdateConstraint(engineForce);
}

void HoverBike::UpdateConstraint(float engineForce) {

	SceneNodeLC::setPosition(Physics::VectorFrom(m_bikeBody->getWorldTransform().getOrigin()));
	//setPosition(m_cubeBody, m_position);

	m_direction = Quaternion::Rotate(Physics::QuaternionFrom(m_bikeBody->getWorldTransform().getRotation()), Vector3f::FORWARD);
	m_direction[1] = 0.0f;	
	setDirection(m_direction);

	if (std::fabs(currentSteering_) > EPSILON) {
		Quaternion rot = m_bikeBody->getWorldTransform().getRotation();
		Vector3f dofdir = Quaternion::Rotate(rot, Vector3f::RIGHT * currentSteering_);
		dofdir[1] = 0.0f;
		if (engineForce < -EPSILON) {
			dofdir *= -1.0f;
		}
		m_direction += dofdir;
	}
	m_direction = Vector3f::Normalize(m_direction);

	//setDirection(m_cubeBody, m_direction);
	pGen6DOFSpring->setFrames(Physics::BtTransform(Physics::QuaternionFrom(Quaternion(m_direction)), Physics::VectorFrom(m_position)), Physics::BtTransform());

	
	// update roll
	/*float velLen = m_bikeBody->getLinearVelocity().length();
	velLen = Math::Clamp(velLen, 0.0f, maxSpeedToMaxRollAngle_);
	float ilerp = Math::InverseLerp(0.0f, maxSpeedToMaxRollAngle_, velLen);
	float roll = ilerp * maxRollAngle_ * std::fabs(currentSteering_) / steeringClamp_;

	if (roll > 0.0f){
		Vector3f rgt = Quaternion::Rotate(m_orientation, Vector3f::RIGHT);
		rgt[1] = 0.0f;
		Vector3f::Normalize(rgt);
		Quaternion nrot = Quaternion(rgt, Vector3f::UP, m_direction);
		Quaternion rollRot = Quaternion(-Math::Sgn(currentSteering_) * roll, Vector3f(0.0f, 0.0f, 1.0f));
		setOrientation(nrot * rollRot);
		//pGen6DOFSpring->setFrames(Physics::BtTransform(Physics::QuaternionFrom(nrot * rollRot), Physics::VectorFrom(m_position)), Physics::BtTransform());
	}*/

		// soft pitch limit
	/*	if (softPitchLimit_ > EPSILON)
		{
			Quaternion brot = Physics::QuaternionFrom(rigidBody_->getWorldTransform().getRotation());
			Constraint6DoF *constraint6DoF = nodeConstraint6DoF_->GetComponent<Constraint6DoF>();
			const Vector3f angLowLimit = constraint6DoF->GetAngularLowerLimit();
			const Vector3f angUppLimit = constraint6DoF->GetAngularUpperLimit();
			Vector3f nangLowLimit(angLowLimit);
			Vector3f nangUppLimit(angUppLimit);

			if (std::fabs(brot.PitchAngle()) > softPitchLimit_)
			{
				nangLowLimit[0] = -M_LARGE_EPSILON;
				nangUppLimit[0] = M_LARGE_EPSILON;
				constraint6DoF->SetAngularLowerLimit(nangLowLimit);
				constraint6DoF->SetAngularUpperLimit(nangUppLimit);
			}
			else if (std::fabs(brot.PitchAngle()) < softPitchLimit_ * 0.2f && nangLowLimit.x_ < nangUppLimit.x_)
			{
				nangLowLimit[0] = 1.0f;
				nangUppLimit[0] = 0.0f;
				constraint6DoF->SetAngularLowerLimit(nangLowLimit);
				constraint6DoF->SetAngularUpperLimit(nangUppLimit);
			}
		}
	*/
}

Vector3f HoverBike::getPosition() {
	return Physics::VectorFrom(m_bikeBody->getWorldTransform().getOrigin());
}

Vector3f HoverBike::getDirection() {
	//Vector3f axis; float angle;
	//m_orientation.toAxisAngle(axis, angle);
	//return axis;
	return Quaternion::Rotate(Physics::QuaternionFrom(m_bikeBody->getWorldTransform().getRotation()), Vector3f::FORWARD);
	
}

Quaternion HoverBike::getOrientation() {
	return Physics::QuaternionFrom(m_bikeBody->getWorldTransform().getRotation());
}

void HoverBike::setPosition(btRigidBody* body, const Vector3f& position) {
	if (body) {
		btTransform& worldTrans = body->getWorldTransform();
		worldTrans.setOrigin(Physics::VectorFrom(position));

		btTransform interpTrans = m_cubeBody->getInterpolationWorldTransform();
		interpTrans.setOrigin(worldTrans.getOrigin());
		body->setInterpolationWorldTransform(interpTrans);
	}
}

void HoverBike::setDirection(btRigidBody* body, const Vector3f& direction) {
	if (body) {
		btTransform& worldTrans = body->getWorldTransform();
		worldTrans.setRotation(Physics::QuaternionFrom(Quaternion(direction)));

		btTransform interpTrans = m_cubeBody->getInterpolationWorldTransform();
		interpTrans.setRotation(worldTrans.getRotation());
		body->setInterpolationWorldTransform(interpTrans);
	}
}

void HoverBike::setDirection(const Vector3f& direction) {
	Quaternion quat = Quaternion(direction);
	SceneNodeLC::setOrientation(quat);
}