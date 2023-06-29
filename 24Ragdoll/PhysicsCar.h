#pragma once

#include "Physics.h"

#define CAR_SCALE 1.0f

struct CarConfig {
	CarConfig() : mass(80.0f),

		/// Amount of torque applied to the wheel. This provides the vehicle's acceleration
		maxEngineForce(1000.0f),  //this should be engine/velocity dependent

		/// Amount of braking torque applied to the wheel.
		maxBreakingForce(100.0f),

		/// Maximum speed clamp (km/h)
		maxSpeedClamp(600.f),

		/// Steering settings
		steeringIncrement(0.025f),
		steeringDecrement(0.1f),
		steeringClamp(0.50f),

		/// Chasis damping
		linearDamping(0.25f), // 0.3
		angularDamping(0.0f),

		/// The direction of ray cast (chassis space). The wheel moves relative to the chassis in this direction, and the suspension force acts along this direction.
		wheelDirectionCS(0.0f, -1.0f, 0.0f),
		/// The axis the wheel rotates around
		wheelAxleCS(-1.0f, 0.0f, 0.0f),

		/// The radius of the wheel
		wheelRadius(0.5f),
		/// The width of the wheel
		wheelWidth(0.4f),

		/// The starting point of the ray, where the suspension connects to the chassis (chassis space)
		connectionWidth(0.725f),  // x
		connectionHeight(1.2f),  // y
		connectionLength(1.325f),  // z 
		bodyConnectionToChasisHeight(1.0f),

		/// The maximum length of the suspension (metres)
		suspensionRestLength(0.6f),

		/// The maximum distance the suspension can be compressed (centimetres)
		suspensionMaxTravelCm(12.0f),

		/// The stiffness constant for the suspension. 10.0 - Offroad buggy, 50.0 - Sports car, 200.0 - F1 Car
		suspensionStiffness(160.f),  //80

		/// The damping coefficient for when the suspension is compressed. Set to k * 2.0 * btSqrt(m_suspensionStiffness) so k is proportional to critical damping.
		/// k = 0.0 undamped & bouncy, k = 1.0 critical damping; k = 0.1 to 0.3 are good values
		suspensionDampingCompression(10.7f),

		/// The damping coefficient for when the suspension is expanding.
		/// DampingRelaxation should be slightly larger than DampingCompression, eg k = 0.2 to 0.5
		suspensionDampingRelaxation(11.7f),  //2.3

											 /// The coefficient of friction between the tyre and the ground.
											 /// Should be about 0.8 for realistic cars, but can increased for better handling.
											 /// Set large (10000.0) for kart racers
		wheelFrictionSlip(250.0f),  //BT_LARGE_FLOAT;

		/// Reduces the rolling torque applied from the wheels that cause the vehicle to roll over.
		/// This is a bit of a hack, but it's quite effective. 0.0 = no roll, 1.0 = physical behaviour.
		/// If m_frictionSlip is too high, you'll need to reduce this to stop the vehicle rolling over.
		/// You should also try lowering the vehicle's centre of mass
		rollInfluence(1.0f)   //1.0f;
	{
		wheelRadius *= CAR_SCALE;
		wheelWidth *= CAR_SCALE;
		connectionHeight *= CAR_SCALE;
		connectionWidth *= CAR_SCALE;
		connectionLength *= CAR_SCALE;   
	}

	btScalar mass, linearDamping, angularDamping;
	btScalar maxEngineForce, maxBreakingForce, maxSpeedClamp, steeringIncrement, steeringDecrement, steeringClamp;
	btScalar wheelFrictionSlip, wheelRadius, wheelWidth, connectionHeight, connectionWidth, connectionLength, bodyConnectionToChasisHeight;
	btVector3 wheelDirectionCS, wheelAxleCS;
	btScalar suspensionStiffness, suspensionDampingRelaxation, suspensionDampingCompression, suspensionRestLength, suspensionMaxTravelCm, rollInfluence;
};

class PhysicsCar {

public:

	PhysicsCar();
	~PhysicsCar();
	
	enum WHEELID {
		WHEEL_FRONTLEFT = 0,
		WHEEL_FRONTRIGHT,
		WHEEL_REARLEFT,
		WHEEL_REARRIGHT,
		WHEEL_COUNT
	};

	void create(const btTransform& transform, int collisionFilterGroup = 1, int collisionFilterMask = -1);
	void update();
	void fixedUpdate();

	btCollisionShape* getWheelShape();
	btRaycastVehicle* getVehicle();
	const btTransform getWorldTransform() const;
	const btVector3& getLinearVelocity() const;

	void turnLeft();
	void turnRight();
	void forward();
	void backward();
	void brake();

private:

	btVehicleRaycaster* m_vehicleRayCaster = nullptr;
	btRaycastVehicle* m_vehicle = nullptr;
	btRigidBody* m_carChassis;
	btCollisionShape* m_wheelShape;

	btTransform m_initialTrans;
	float m_engineForce;
	float m_breakingForce;
	float m_vehicleSteering;
	bool m_turned;

	CarConfig m_cfg;
};