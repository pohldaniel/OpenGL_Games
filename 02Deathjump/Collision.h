#pragma once
#include "Collider.h"

class Collision {
public:
	Collision(Collider& body);
	~Collision();

	bool checkCollision(const Collision& other, Vector2f& MTV) const;
	bool checkCollision(const Collision& other) const;
	const Collider getBody();
private:
	Collider& body;

	const static Vector2f abs(Vector2f v);
};