#pragma once
#include "Collision.h"

//TODO: REMOVE DRAWABLE
class Wall {
public:
	Wall(Vector2f position, Vector2f size);
	~Wall();

	Collision GetCollider();

private:
	Collider m_collider;

	void InitCollider(const Vector2f& position, const Vector2f& size);
};