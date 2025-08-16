#pragma once
#pragma once

#include <btBulletDynamicsCommon.h>

class KinematicObject {

public:

	KinematicObject();
	~KinematicObject();

	virtual void create(btCollisionShape* shape, const btTransform& transform, btDynamicsWorld* physicsWorld, int collisionFilterGroup = 1, int collisionFilterMask = -1, void* userPointer = NULL);
	btRigidBody* getRigidBody();
	void setUserPointer(void* userPointer);

protected:

	btRigidBody* m_rigidBody;
};