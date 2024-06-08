#pragma once
#include <Physics/Physics.h>

class VehiclePhysics {

public:
	VehiclePhysics();
	~VehiclePhysics();
	btRaycastVehicle* vehicle;

	// Probably you'll need methods like:
	void ApplyEngineForce(float force);
	void ApplySteer(float value);
	void Brake(float force);
	std::string debugStateSTR();
	void Update();  // For updating any vehicle logic, physics simulation steps, etc.
	btTransform GetTransform() const;  // Useful for rendering

	float getX() const;
	float getY() const;
	float getZ() const;

private:
	btRigidBody* vehicleRigidBody;
	btVehicleRaycaster* vehicleRayCaster;

	btDefaultMotionState *vehicleMotionState;

	btRaycastVehicle::btVehicleTuning tuning;

	btScalar VEHICLE_SCALE;

	float engineForce;
	float vehicleSteering;
	float steeringIncrement;
	float steeringClamp;
	float brakeForce;

	//Will make this slowly incr/decr for steer inputs
	float currentSteer;


};