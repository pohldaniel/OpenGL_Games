#pragma once
#include <engine/MeshObject/Shape.h>
#include <Physics/Physics.h>

#include "Entities/ShapeEntity.h"

enum D6LimitIndexType {
	D6_LINEAR_X = 0,
	D6_LINEAR_Y,
	D6_LINEAR_Z,
	D6_ANGULAR_X,
	D6_ANGULAR_Y,
	D6_ANGULAR_Z,
};

class HoverBike : public ShapeEntity {


public:
	HoverBike(const Shape& shape, const Camera& camera);
	virtual ~HoverBike();
	virtual void DelayedStart();
	virtual void FixedUpdate(float timeStep);

	const Vector3f& getDirection();
	void setPosition(const float x, const float y, const float z) override;
	void setPosition(const Vector3f& position) override;
	void setOrientation(const Quaternion& orientation) override;
	void setDirection(const Vector3f& direction);
	bool Create();
	void postUpdate();

protected:
	void CreateRaycastVehicle();
	void UpdateConstraint(float engineForce);

protected:
	float engineForce_;
	float maxSpeed_;
	float minBrakeForce_;
	float maxBrakeForce_;
	float minBrakeSpeed_;
	float wheelRadius_;
	float wheelFriction_;

	float steeringIncrement_;
	float steeringClamp_;
	float maxRollAngle_;
	float maxSpeedToMaxRollAngle_;

	float suspensionRestLength_;
	float suspensionStiffness_;
	float suspensionRelaxation_;
	float suspensionCompression_;
	float rollInfluence_;
	std::string constraintName_;
	float softPitchLimit_;


	btRaycastVehicle::btVehicleTuning   vehicleTuning_;
	btVehicleRaycaster                  *vehicleRaycaster_;
	btRaycastVehicle                    *raycastVehicle_;
	//btGeneric6DofSpring2Constraint* pGen6DOFSpring;
	btGeneric6DofConstraint* pGen6DOFSpring;
	btRigidBody*                        m_bikeBody;
	btRigidBody*                        m_cubeBody;
	float currentSteering_;
	Vector3f m_direction;
};