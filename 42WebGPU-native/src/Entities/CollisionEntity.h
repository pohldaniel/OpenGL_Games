#pragma once

#include <Physics/Physics.h>
#include <engine/scene/CollisionNode.h>
#include "Entity.h"

class CollisionEntity : public CollisionNode, public Entity {

public:

	CollisionEntity(btCollisionObject* collisionObject);
	~CollisionEntity();

	void update(const float dt) override;
	void fixedUpdate(float fdt) override;
};