#pragma once
#include <Physics/Physics.h>
#include "SplinePath.h"

class SplinePlatform {

public:

	SplinePlatform();
	~SplinePlatform();
	void fixedUpdate(float fdt);

	void initialize(SplinePath* splinePath, btCollisionObject* collisionShape);

	btCollisionObject* getCollisionShape();

protected:

	SplinePath* m_splinePath;
	btCollisionObject* m_collisionShape;
	SceneNodeLC* m_controlNode;

	Vector3f m_prevPos;
	Vector3f m_curPos;
	Matrix4f m_prevMat;
	Matrix4f m_curMat;
	float m_rotation;
};