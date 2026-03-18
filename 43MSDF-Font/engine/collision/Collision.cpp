#include <iostream>
#include "Collision.h"

Collision::Collision(Collider& body) : body(body){ }

Collision::~Collision() {

}

bool Collision::checkCollision(const Collision& other, Vector2f& MTV) const {
	const Vector2f otherPosition = other.body.position;
	const Vector2f otherSize = other.body.size;

	const Vector2f thisPosition = this->body.position;
	const Vector2f thisSize = this->body.size;

	bool collisionX = (thisPosition[0] + thisSize[0] >= otherPosition[0]) && (otherPosition[0] + otherSize[0] >= thisPosition[0]);
	bool collisionY = (thisPosition[1] + thisSize[1] >= otherPosition[1]) && (otherPosition[1] + otherSize[1] >= thisPosition[1]);
	
	if (collisionX && collisionY) {

		float topDistance = (otherPosition[1] + otherSize[1]) - thisPosition[1];
		float bottomDistance = (thisPosition[1] + thisSize[1]) - otherPosition[1];
		float intersectY = std::min(topDistance, bottomDistance);

		float leftDistance = (thisPosition[0] + thisSize[0]) - otherPosition[0];
		float rightDistance = (otherPosition[0] + otherSize[0]) - thisPosition[0];
		float intersectX = std::min(leftDistance, rightDistance);

		if (intersectX < intersectY) {

			if (leftDistance < rightDistance) {
				MTV[0] = otherPosition[0] - (thisPosition[0] + thisSize[0]);
			}else {
				MTV[0] = (otherPosition[0] + otherSize[0]) - thisPosition[0];
			}
			MTV[1] = 0.0f;
		}else if(intersectY < intersectX){
			if (topDistance < bottomDistance) {
				MTV[1] = ((otherPosition[1] + otherSize[1]) - thisPosition[1]);
			}else {
				MTV[1] = otherPosition[1] - (thisPosition[1] + thisSize[1]);
			}
			MTV[0] = 0.0f;
		}else {
			MTV[0] = 0.0f;
			MTV[1] = 0.0f;
		}
		return true;
	}
	return false;
}

bool Collision::checkCollision(const Collision& other) const {
	const Vector2f otherPosition = other.body.position;
	const Vector2f otherSize = other.body.size;

	const Vector2f thisPosition = this->body.position;
	const Vector2f thisSize = this->body.size;

	bool collisionX = (thisPosition[0] + thisSize[0] >= otherPosition[0]) && (otherPosition[0] + otherSize[0] >= thisPosition[0]);
	bool collisionY = (thisPosition[1] + thisSize[1] >= otherPosition[1]) && (otherPosition[1] + otherSize[1] >= thisPosition[1]);

	if (collisionX && collisionY) {

		float topDistance = (otherPosition[1] + otherSize[1]) - thisPosition[1];
		float bottomDistance = (thisPosition[1] + thisSize[1]) - otherPosition[1];
		float intersectY = std::min(topDistance, bottomDistance);

		float leftDistance = (thisPosition[0] + thisSize[0]) - otherPosition[0];
		float rightDistance = (otherPosition[0] + otherSize[0]) - thisPosition[0];
		float intersectX = std::min(leftDistance, rightDistance);

		return true;
	}
	return false;
}

const Collider Collision::getBody() {
	return body;
}