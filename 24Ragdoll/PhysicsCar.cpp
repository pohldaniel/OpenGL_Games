#include "engine/input/Keyboard.h"
#include "PhysicsCar.h"

PhysicsCar::PhysicsCar() : m_engineForce(0.f), m_breakingForce(0.f), m_vehicleSteering(0.f), m_turned(false) {

}

PhysicsCar::~PhysicsCar() {
	delete m_vehicleRayCaster;
	delete m_vehicle;
}

void PhysicsCar::create(const btTransform& transform, float mass, int collisionFilterGroup, int collisionFilterMask, void* rigidBodyUserPointer) {
	m_initialTrans = transform;
	btCollisionShape* chassisShape = new btBoxShape(btVector3(1.0f, 0.5f, 2.0f));


	btCompoundShape* compound = new btCompoundShape();
	btTransform localTrans;
	localTrans.setIdentity();
	localTrans.setOrigin(btVector3(0.0f, m_cfg.bodyConnectionToChasisHeight, 0.0f));
	compound->addChildShape(localTrans, chassisShape);

	m_carChassis = Physics::AddRigidBody(m_cfg.mass, transform, compound);
	m_carChassis->setDamping(m_cfg.linearDamping, m_cfg.angularDamping);

	m_wheelShape = new btCylinderShapeX(btVector3(m_cfg.wheelWidth, m_cfg.wheelRadius, m_cfg.wheelRadius));

	btRaycastVehicle::btVehicleTuning tuning;
	m_vehicleRayCaster = new btDefaultVehicleRaycaster(Physics::GetDynamicsWorld());
	m_vehicle = new btRaycastVehicle(tuning, m_carChassis, m_vehicleRayCaster);
	m_carChassis->setActivationState(DISABLE_DEACTIVATION);

	Physics::GetDynamicsWorld()->addVehicle(m_vehicle);


	bool isFrontWheel = true;
	m_vehicle->setCoordinateSystem(0, 1, 2);

	btVector3 connectionPointCS0(1.0f - (0.3f * m_cfg.wheelWidth), m_cfg.connectionHeight, 2.0f  - m_cfg.wheelRadius);
	m_vehicle->addWheel(connectionPointCS0, m_cfg.wheelDirectionCS, m_cfg.wheelAxleCS, m_cfg.suspensionRestLength, m_cfg.wheelRadius, tuning, isFrontWheel);
	connectionPointCS0 = btVector3((0.3f * m_cfg.wheelWidth) - 1.0f, m_cfg.connectionHeight, 2.0f - m_cfg.wheelRadius);
	m_vehicle->addWheel(connectionPointCS0, m_cfg.wheelDirectionCS, m_cfg.wheelAxleCS, m_cfg.suspensionRestLength, m_cfg.wheelRadius, tuning, isFrontWheel);
	connectionPointCS0 = btVector3( (0.3f * m_cfg.wheelWidth) - 1.0f, m_cfg.connectionHeight, m_cfg.wheelRadius - 2.0f);
	isFrontWheel = false;
	m_vehicle->addWheel(connectionPointCS0, m_cfg.wheelDirectionCS, m_cfg.wheelAxleCS, m_cfg.suspensionRestLength, m_cfg.wheelRadius, tuning, isFrontWheel);
	connectionPointCS0 = btVector3(1.0f - (0.3f * m_cfg.wheelWidth), m_cfg.connectionHeight, m_cfg.wheelRadius - 2.0f);
	m_vehicle->addWheel(connectionPointCS0, m_cfg.wheelDirectionCS, m_cfg.wheelAxleCS, m_cfg.suspensionRestLength, m_cfg.wheelRadius, tuning, isFrontWheel);
	
	for (int i = 0; i<m_vehicle->getNumWheels(); i++) {
		btWheelInfo& wheel = m_vehicle->getWheelInfo(i);
		wheel.m_suspensionStiffness = m_cfg.suspensionStiffness;
		wheel.m_wheelsDampingRelaxation = m_cfg.suspensionDampingRelaxation;
		wheel.m_wheelsDampingCompression = m_cfg.suspensionDampingCompression;
		wheel.m_frictionSlip = m_cfg.wheelFrictionSlip;
		wheel.m_rollInfluence = m_cfg.rollInfluence;
		wheel.m_maxSuspensionTravelCm = m_cfg.suspensionMaxTravelCm;
	}
}

void PhysicsCar::fixedUpdate() {
	// engine
	m_vehicle->applyEngineForce(m_engineForce, WHEEL_REARLEFT);
	m_vehicle->applyEngineForce(m_engineForce, WHEEL_REARRIGHT);

	// brakes
	m_vehicle->setBrake(m_breakingForce, WHEEL_REARLEFT);
	m_vehicle->setBrake(m_breakingForce, WHEEL_REARLEFT);

	// steering
	m_vehicle->setSteeringValue(m_vehicleSteering, WHEEL_FRONTLEFT);
	m_vehicle->setSteeringValue(m_vehicleSteering, WHEEL_FRONTRIGHT);

	m_engineForce = 0.f;
	m_breakingForce = 0.f;

	// update
	if (!m_turned)
	{
		if (m_vehicleSteering > 0.f)
		{
			m_vehicleSteering -= m_cfg.steeringDecrement;
			if (m_vehicleSteering < 0.f)
				m_vehicleSteering = 0.f;
		}
		else
		{
			m_vehicleSteering += m_cfg.steeringDecrement;
			if (m_vehicleSteering > 0.f)
				m_vehicleSteering = 0.f;
		}
	}
	m_turned = false;
}

void PhysicsCar::update() {
	Keyboard &keyboard = Keyboard::instance();

	if (keyboard.keyDown(Keyboard::KEY_UP)) {
		forward();
	}

	if (keyboard.keyDown(Keyboard::KEY_DOWN)) {
		backward();
	}

	if (keyboard.keyDown(Keyboard::KEY_LEFT)) {
		turnLeft();
	}

	if (keyboard.keyDown(Keyboard::KEY_RIGHT)) {
		turnRight();
	}
}

btCollisionShape* PhysicsCar::getWheelShape() {
	return m_wheelShape;
}

btRaycastVehicle* PhysicsCar::getVehicle() {
	return m_vehicle;
}

const btTransform PhysicsCar::getWorldTransform() const {
	btTransform vehicleTrans;
	m_vehicle->getRigidBody()->getMotionState()->getWorldTransform(vehicleTrans);
	btCompoundShape* compoundShape = static_cast<btCompoundShape*>(m_vehicle->getRigidBody()->getCollisionShape());

	return vehicleTrans * compoundShape->getChildTransform(0);
}

const btVector3& PhysicsCar::getLinearVelocity() const {
	return m_vehicle->getRigidBody()->getLinearVelocity();
}

void PhysicsCar::turnLeft() {

	if (m_vehicleSteering < 0)
		m_vehicleSteering += m_cfg.steeringDecrement;
	else
		m_vehicleSteering += m_cfg.steeringIncrement;

	if (m_vehicleSteering > m_cfg.steeringClamp)
		m_vehicleSteering = m_cfg.steeringClamp;

	m_turned = true;
}

void PhysicsCar::turnRight() {

	if (m_vehicleSteering > 0)
		m_vehicleSteering -= m_cfg.steeringDecrement;
	else
		m_vehicleSteering -= m_cfg.steeringIncrement;

	if (m_vehicleSteering < -m_cfg.steeringClamp)
		m_vehicleSteering = -m_cfg.steeringClamp;

	m_turned = true;
}

void PhysicsCar::forward() {
	if (std::abs(m_vehicle->getCurrentSpeedKmHour()) < m_cfg.maxSpeedClamp)
		m_engineForce = m_cfg.maxEngineForce;

	m_breakingForce = 0.f;
}

void PhysicsCar::backward() {
	if (std::abs(m_vehicle->getCurrentSpeedKmHour()) < m_cfg.maxSpeedClamp)
		m_engineForce = -m_cfg.maxEngineForce;

	m_breakingForce = 0.f;
}

void PhysicsCar::brake() {
	m_breakingForce = m_cfg.maxBreakingForce;
	m_engineForce = 0.f;
}