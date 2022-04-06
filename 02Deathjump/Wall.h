#pragma once
#include "engine/collision/Collision.h"
#include "Constants.h"

class Wall {
public:
	Wall(Vector2f position, Vector2f size);
	~Wall();
	void render();
	Collision getCollider();

private:
	Collider m_collider;

	void initCollider(const Vector2f& position, const Vector2f& size);
};