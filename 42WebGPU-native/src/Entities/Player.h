#pragma once

#include <Physics/Physics.h>
#include <engine/scene/CollisionNode.h>
#include <engine/animation/AnimatedModel.h>
#include "Entity.h"

class Player : public CollisionNode, public Entity {

public:

	Player(btCollisionObject* collisionObject, AnimatedModel& model);
	~Player();

	void update(const float dt) override;
	void fixedUpdate(float fdt) override;

	void translate(float dx, float dy, float dz)  override;
	const Vector3f& getPosition() const override;

private:

	AnimatedModel& model;
};