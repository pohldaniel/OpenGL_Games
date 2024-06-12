#pragma once
#include <Physics/Physics.h>

class VehiclePhysics {

public:
	VehiclePhysics();
	~VehiclePhysics();

	void update();

	void applyEngineForce(float force);
	void applySteer(float value);
	void brake(float force);

	std::string debugStateSTR();
	btTransform getTransform() const;
	const int getNumWheels() const;
	const btWheelInfo& getWheelInfo(const int index) const;
	btRigidBody* getRigidBody();
	btRaycastVehicle* m_vehicle;
private:

	btRigidBody* m_vehicleRigidBody;
	btVehicleRaycaster* m_vehicleRayCaster;
	btDefaultMotionState* m_vehicleMotionState;
	btRaycastVehicle::btVehicleTuning m_tuning;
	btScalar VEHICLE_SCALE;
	

	float m_engineForce;
	float m_vehicleSteering;
	float m_steeringIncrement;
	float m_steeringClamp;
	float m_brakeForce;
	float m_currentSteer;
};