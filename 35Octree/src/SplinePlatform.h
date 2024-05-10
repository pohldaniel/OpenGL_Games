#pragma once
#include <Physics/Physics.h>
#include "SplinePath.h"

class SplinePlatform {

public:

	SplinePlatform();
	~SplinePlatform();
	void fixedUpdate(float fdt);

	void initialize(SplinePath* splinePath, btCollisionObject* collisionObject);

	btCollisionObject* getCollisionObject();

protected:

	SplinePath* m_splinePath;
	btCollisionObject* m_collisionObject;
	SceneNodeLC* m_controlNode;

	Vector3f m_prevPos;
	Vector3f m_curPos;
	Matrix4f m_prevMat;
	Matrix4f m_curMat;
	float m_rotation;
};