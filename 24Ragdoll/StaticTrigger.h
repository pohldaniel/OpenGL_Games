#pragma once

#include <btBulletDynamicsCommon.h>
#include "engine/Vector.h"

class StaticTrigger {

public:

	StaticTrigger();
	~StaticTrigger();

	virtual void create(btCollisionShape* shape, const btTransform& transform, btDynamicsWorld* physicsWorld, int collisionFilterGroup = 1, int collisionFilterMask = -1, void* rigidBodyUserPointer = NULL);
	btCollisionObject* getCollisionObject();

protected:

	btCollisionObject* m_collisionObject;
};