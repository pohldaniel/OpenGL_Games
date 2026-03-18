#include <iostream>
#include "VehiclePhysics.h"
#include "Globals.h"

VehiclePhysics::VehiclePhysics() : m_vehicle(nullptr){
	//PhysicsWorldSingleton *physicsWorld = PhysicsWorldSingleton::getInstance();

	VEHICLE_SCALE = 0.9;

	// Vehicle tuning:
	m_tuning.m_suspensionStiffness = 90.0f;
	m_tuning.m_suspensionCompression = 0.83f;
	m_tuning.m_suspensionDamping = 0.88f;
	m_tuning.m_maxSuspensionTravelCm = 100.0f;
	m_tuning.m_frictionSlip = 10.5f;
	m_tuning.m_maxSuspensionForce = 6000.0f;

	// Vehicle setup
	btBoxShape *vehicleChassisShape = new btBoxShape(btVector3(1.6f * VEHICLE_SCALE, 0.5f * VEHICLE_SCALE, 3.0f * VEHICLE_SCALE));
	m_vehicleMotionState = new btDefaultMotionState();
	btTransform localTransform;

	localTransform.setIdentity();
	//localTransform.setOrigin(btVector3(0, 20, -120));
	localTransform.setOrigin(btVector3(0, 40, -240));

	m_vehicleMotionState->setWorldTransform(localTransform);

	//* VEHICLE MASS !
	btScalar vehicleMass = 800;

	btVector3 vehicleInertia(0, 0, 0);
	vehicleChassisShape->calculateLocalInertia(vehicleMass, vehicleInertia);
	btRigidBody::btRigidBodyConstructionInfo vehicleRigidBodyCI(vehicleMass, m_vehicleMotionState, vehicleChassisShape, vehicleInertia);

	m_vehicleRigidBody = new btRigidBody(vehicleRigidBodyCI);
	//vehicleRigidBody->setAngularFactor(btVector3(0.0f, 1.0f, 0.0f));

	Physics::GetDynamicsWorld()->addRigidBody(m_vehicleRigidBody, Physics::CAR, Physics::FLOOR);

	// vehicleRigidBody->getWorldTransform
	// ^ use for rotation camera thingy


	// Raycaster and the actual vehicle
	m_vehicleRayCaster = new btFilteredVehicleRaycaster(Physics::GetDynamicsWorld(), Physics::CAR, Physics::FLOOR);
	m_vehicle = new btRaycastVehicle(m_tuning, m_vehicleRigidBody, m_vehicleRayCaster);

	btVector3 wheelDirection = btVector3(0, -1, 0);
	btVector3 wheelAxle = btVector3(-1.0, 0, 0); //used to be -1
	btScalar suspensionRestLength = 0.3;
	btScalar wheelRadius = 1.5*VEHICLE_SCALE;
	btScalar wheelWidth = 0.4;
	btScalar suspensionStiffness = 40.0;
	btScalar dampingRelaxation = 4.3;
	btScalar dampingCompression = 2.4;
	btScalar frictionSlip = 10;
	btScalar rollInfluence = 0.1;

	//* Adding WHEELS to vehicle physics model !

	//!This like, fixes the wonky vehicle sim lmao
	m_vehicle->setCoordinateSystem(0, 1, 2);

	auto halfExtents = vehicleChassisShape->getHalfExtentsWithoutMargin();
	btScalar connectionHeight(3.2);

	btVector3 wheelConnectionPoint(halfExtents.x() - 0.4, connectionHeight, halfExtents.z() - 0.5);

	// Adds the front wheels
	m_vehicle->addWheel(wheelConnectionPoint * btVector3(2, 0, 1), wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, m_tuning, true);

	m_vehicle->addWheel(wheelConnectionPoint * btVector3(-2, 0, 1), wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, m_tuning, true);

	// Adds the rear wheels
	m_vehicle->addWheel(wheelConnectionPoint * btVector3(2, 0, -1), wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, m_tuning, false);

	m_vehicle->addWheel(wheelConnectionPoint * btVector3(-2, 0, -1), wheelDirection, wheelAxle, suspensionRestLength, wheelRadius, m_tuning, false);

	for (int i = 0; i < m_vehicle->getNumWheels(); i++){
		btWheelInfo &wheel = m_vehicle->getWheelInfo(i);

		// Larger 
		//wheel.m_chassisConnectionPointCS = wheelConnectionPoint;

		wheel.m_suspensionStiffness = suspensionStiffness;
		wheel.m_wheelsDampingRelaxation = dampingRelaxation;

		// wheel.m_wheelsDampingCompression = dampingCompression;
		wheel.m_wheelsDampingCompression = btScalar(0.3) * 2 * btSqrt(wheel.m_suspensionStiffness); // btScalar(0.8);
		wheel.m_wheelsDampingRelaxation = btScalar(0.5) * 2 * btSqrt(wheel.m_suspensionStiffness);  // 1;

	   // Larger friction slips will result in better handling
		wheel.m_frictionSlip = frictionSlip;
		wheel.m_rollInfluence = rollInfluence;

	}

	m_vehicleRigidBody->setActivationState(DISABLE_DEACTIVATION);

	//! Experimental
	// vehicleRigidBody->setLinearVelocity(btVector3(0, 0, 0));
	// vehicleRigidBody->setAngularVelocity(btVector3(0, 0, 0));

	Physics::GetDynamicsWorld()->addVehicle(m_vehicle);

	// Vehicle setup:
	m_engineForce = 0.0f;
	m_vehicleSteering = 0.0f;
	m_steeringIncrement = 0.04f;
	m_steeringClamp = 0.35f;
	m_brakeForce = 0.0f;
}

VehiclePhysics::~VehiclePhysics() {

	if (m_vehicleRayCaster) {
		delete m_vehicleRayCaster;
		m_vehicleRayCaster = nullptr;
	}

	if (m_vehicle) {
		Physics::GetDynamicsWorld()->removeVehicle(m_vehicle);
		Physics::DeleteCollisionObject(m_vehicleRigidBody);
		delete m_vehicle;
		m_vehicle = nullptr;
	}
}

void VehiclePhysics::applyEngineForce(float force){
	m_engineForce = force;
	// Rear Wheel Drive
	m_vehicle->applyEngineForce(m_engineForce, 0);
	m_vehicle->applyEngineForce(m_engineForce, 1);
	// TODO: Add any Bullet physics code here that applies this force
}

void VehiclePhysics::applySteer(float steerIncr){
	if (steerIncr != 0){
		// Increment or decrement steering based on steerIncr
		m_vehicleSteering += steerIncr;

		// Clamp the steering value to make sure it's within [-steeringClamp, steeringClamp]
		if (m_vehicleSteering > m_steeringClamp) {
			m_vehicleSteering = m_steeringClamp;
		}else if (m_vehicleSteering < -m_steeringClamp) {
			m_vehicleSteering = -m_steeringClamp;
		}
	}else{
		// Tend the steering value back to zero if steerIncr is 0
		if (m_vehicleSteering > m_steeringIncrement) {
			m_vehicleSteering -= m_steeringIncrement;
		}else if (m_vehicleSteering < -m_steeringIncrement) {
			m_vehicleSteering += m_steeringIncrement;
		}else {
			m_vehicleSteering = 0; // Close enough to zero we can set it directly
		}
	}

	// Apply the steering value to the front wheels
	m_vehicle->setSteeringValue(m_vehicleSteering, 0);
	m_vehicle->setSteeringValue(m_vehicleSteering, 1);
}


void VehiclePhysics::brake(float force){
	m_brakeForce = force;
	m_vehicle->setBrake(m_brakeForce, 2);
	m_vehicle->setBrake(m_brakeForce, 3);
}

void VehiclePhysics::update(){
	
}

btTransform VehiclePhysics::getTransform() const{
	btTransform trans;
	m_vehicleMotionState->getWorldTransform(trans);
	return trans;
}

const int VehiclePhysics::getNumWheels() const {
	return m_vehicle->getNumWheels();
}

const btWheelInfo& VehiclePhysics::getWheelInfo(const int index) const {
	return m_vehicle->getWheelInfo(index);
}

btRigidBody* VehiclePhysics::getRigidBody() {
	return m_vehicle->getRigidBody();
}

std::string VehiclePhysics::debugStateSTR(){
	btVector3 velocity = m_vehicleRigidBody->getLinearVelocity();
	btVector3 vehiclePosition = m_vehicleRigidBody->getWorldTransform().getOrigin();

	char debugVehicle_STR[1024];
	sprintf(debugVehicle_STR,
		"(X,Y,Z)\n\n"
		"POS=      %.2f , %.2f , %.2f\n\n"
		"VELOCITY= %.2f , %.2f , %.2f\n\n"
		"STEER=    %.2f , %.2f",
		vehiclePosition.getX(), vehiclePosition.getY(), vehiclePosition.getZ(),
		velocity.getX(), velocity.getY(), velocity.getZ(),
		m_vehicle->getSteeringValue(0), m_vehicle->getSteeringValue(1));

	return std::string(debugVehicle_STR);
}