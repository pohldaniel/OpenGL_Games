#pragma once

#include <btBulletDynamicsCommon.h>
#include "engine/Vector.h"
#include "RenderableObject.h"

#define FLOW_SPEED 2.0f
#define LAVA_HEIGHT_MAX 4.0f
#define LAVA_HEIGHT_MIN 1.0f

class Lava : public RenderableObject{

public:

	Lava();
	~Lava();

	void draw(const Camera& camera) override;
	void create(btCollisionShape* shape, const btTransform& transform, btDynamicsWorld* physicsWorld, int collisionFilterGroup = 1, int collisionFilterMask = -1, void* rigidBodyUserPointer = NULL);
	float getHeight();
	float getHeightMax();
	btCollisionObject* getCollisionObject();

	static void TickCallback(btDynamicsWorld* world, btScalar timeStep);

private:
	
	void updateHeight();

	float m_height;
	float height, ang;
	bool up;
	btCollisionObject* m_collisionObject;
};