
#include "Collision.h"

Collision::Collision(Collider& body) : body(body){ }

Collision::~Collision() {

}

bool Collision::checkCollision(const Collision& other, Vector2f& MTV) const {
	const Vector2f otherPosition = other.body.position;
	const Vector2f otherSize = other.body.size / 2.0f;

	const Vector2f thisPosition = this->body.position;
	const Vector2f thisSize = this->body.size / 2.0f;

	const Vector2f delta = otherPosition - thisPosition;
	const Vector2f intersect = abs(delta) - (otherSize + thisSize);

	if (intersect[0] < 0.0f && intersect[1] < 0.0f) {
		if (intersect[0] > intersect[1])
			MTV[0] = (delta[0] > 0.0f ? intersect[0] : -intersect[0]);
		else
			MTV[1] = (delta[1] > 0.0f ? intersect[1] : -intersect[1]);
		return true;
	}

	return false;
}

const Vector2f Collision::abs(Vector2f v) {
	v[0] = (v[0] < 0.0f ? -v[0] : v[0]);
	v[1] = (v[1] < 0.0f ? -v[1] : v[1]);

	return v;
}

const Collider Collision::getBody() {
	return body;
}