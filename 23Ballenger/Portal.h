#pragma once

#include <btBulletDynamicsCommon.h>
#include "engine/Camera.h"
#include "RenderableObject.h"

#define PORTAL_SIDE  3.0f
#define PORTAL_SPEED 2.0f

class Terrain;

class Portal : public RenderableObject {

public:

	Portal();
	~Portal();

	void init(const Terrain& terrain);
	void create(btCollisionShape* shape, const btTransform& transform, btDynamicsWorld* physicsWorld, int collisionFilterGroup, int collisionFilterMask, void* rigidBodyUserPointer = NULL);
	void draw(const Camera& camera) override;
	void update(const float dt) override;
	void setDisabled(bool disabled);

	const Vector3f getReceptor(int index) const;
	float getZ();
	btCollisionObject* getCollisionObject();

private:

	RenderableObject m_vortex;
	btCollisionObject* m_collisionObject;
	Vector3f m_debugPosition;
};