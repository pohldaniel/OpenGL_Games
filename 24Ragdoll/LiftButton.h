#pragma once

#include <btBulletDynamicsCommon.h>
#include "engine/Vector.h"
#include "RenderableObject.h"

class LiftButton : public RenderableObject {

public:

	LiftButton();
	~LiftButton();

	void draw(const Camera& camera) override;
	void create(btCollisionShape* shape, const btTransform& transform, btDynamicsWorld* physicsWorld, int collisionFilterGroup = 1, int collisionFilterMask = -1, void* rigidBodyUserPointer = NULL);
	const float getHeight() const;
	float getHeightMax();
	btCollisionObject* getCollisionObject();

	static void TickCallback(btDynamicsWorld* world, btScalar timeStep);

private:

	void updateHeight();

	btCollisionObject* m_collisionObject;
};