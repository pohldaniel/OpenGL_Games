#pragma once

#include <engine/scene/SequenceNode.h>
#include <Entities/Entity.h>
#include <Physics/VehiclePhysics.h>

class Vehicle : public SequenceNode, public Entity {

public:

	Vehicle(const MeshSequence& meshSequence);
	virtual ~Vehicle();

	virtual void draw() override;

	const Vector3f getPosition();
	const Matrix4f getWorldTransform();
	const Vector3f getLinearVelocity();
	void roate(float x, float y, float z);
	void applyEngineForce(float force);
	void applySteer(float steer);

private:

	void updateModelMatrix();
	std::vector<Matrix4f> m_wheelMatrices;
	Matrix4f m_objModelMatrix;
	VehiclePhysics m_vehicle;
};