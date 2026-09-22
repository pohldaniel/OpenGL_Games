#pragma once

#include <Physics/Physics.h>
#include <engine/scene/CollisionNode.h>
#include "Entity.h"

class Enemy : public CollisionNode, public Entity {

public:

	Enemy(btCollisionObject* collisionObject, const Vector3f& target);
	~Enemy();

	void update(const float dt) override;
	void fixedUpdate(float fdt) override;

	const Vector3f getDirection() const;
	void setIsDeath(bool isDeath);
	bool isDeath();

private:

	float getLookAtYRotation(const Vector3f& objectPos, const Vector3f& targetPos);

	const Vector3f& target;
	bool m_isDeath;
};