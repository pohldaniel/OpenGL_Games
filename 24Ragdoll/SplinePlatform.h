#pragma once
#include "Physics.h"
#include "turso3d/Scene/SplinePath.h"

class SplinePlatform {

public:

	SplinePlatform();
	~SplinePlatform();
	void FixedUpdate(float timeStep);

	void Initialize(SplinePath* splinePathdel, btRigidBody* body);
	
	btRigidBody* getRigidBody();

protected:

	WeakPtr<SplinePath> splinePath_;
	btRigidBody* rigidBody_;
	WeakPtr<SpatialNode> controlNode_;

	Vector3 prevPos_;
	Vector3 curPos_;
	Matrix3x4 prevMat_;
	Matrix3x4 curMat_;
	float rotation_;
};