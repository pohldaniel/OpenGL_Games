#pragma once
#include "Collider.h"

class Collision {
public:
	Collision(Collider& body);
	~Collision();

	bool CheckCollision(const Collision& other, Vector2f& MTV) const;

	const Collider GetBody();
private:
	Collider& body;

	const static Vector2f abs(Vector2f v);
};