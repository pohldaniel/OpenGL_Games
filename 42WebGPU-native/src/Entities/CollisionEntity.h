#pragma once

#include <Physics/Physics.h>
#include <engine/scene/CollisionNode.h>
#include "Entity.h"

class CollisionEntity : public CollisionNode, public Entity {

public:

	CollisionEntity(btCollisionObject* collisionObject);
	~CollisionEntity();

	void update(float dt) override;
	void fixedUpdate(float fdt) override;

	void setPosition(const float x, const float y, const float z) const override;
	void setOrientation(const float x, const float y, const float z, const float w) const override;
};