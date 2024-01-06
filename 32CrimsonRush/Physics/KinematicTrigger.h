#pragma once

#include <btBulletDynamicsCommon.h>
#include "engine/Vector.h"

class KinematicTrigger {

public:

	KinematicTrigger();
	~KinematicTrigger();

	virtual void create(btCollisionShape* shape, const btTransform& transform, btDynamicsWorld* physicsWorld, int collisionFilterGroup = 1, int collisionFilterMask = -1, void* userPointer = NULL);
	btCollisionObject* getCollisionObject();
	void setUserPointer(void* userPointer);

protected:

	btCollisionObject* m_collisionObject;
};