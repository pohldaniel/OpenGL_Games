#pragma once
#include <engine/Texture.h>
#include <engine/Camera.h>
#include <engine/Material.h>
#include <engine/scene/SequenceNode.h>
#include <Entities/Entity.h>
#include <Physics/VehiclePhysics.h>

class Vehicle : public SequenceNode, public Entity {

public:

	Vehicle(const MeshSequence& meshSequence);
	virtual ~Vehicle();

	virtual void draw();

	const btTransform& getWorldTransform() const;
	const btVector3& getLinearVelocity() const;
	void roate(float x, float y, float z);
	VehiclePhysics vehicle;

private:

	void updateModelMatrix();
	std::vector<Matrix4f> wheelMatrices;
	Matrix4f objModelMatrix;
};