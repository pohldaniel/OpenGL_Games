#pragma once

#include <Physics/Physics.h>
#include <engine/scene/CollisionNode.h>
#include "Entity.h"

class Enemy : public CollisionNode, public Entity {

public:

	Enemy(btCollisionObject* collisionObject);
	~Enemy();

	void update(const float dt) override;
	void fixedUpdate(float fdt) override;
};