#include <iostream>
#include "Platform.h"

Platform::Platform(CollisionRect& collisionRect, MoveDirection moveDirection, float speed, const Vector2f& start, const Vector2f& end) :
	EntityNew(), 
	collisionRect(collisionRect),
	m_moveDirection(moveDirection),
	m_speed(speed),
    m_start(start),
	m_end(end),
	m_direction(1.0f){

}

Platform::~Platform() {
	
}

void Platform::update(float dt) {
	collisionRect.previousRect = { collisionRect.posX, collisionRect.posY, collisionRect.width, collisionRect.height, collisionRect.hasCollision };

	m_moveDirection == MoveDirection::P_HORIZONTAL ? collisionRect.posX += m_direction * m_speed * dt : collisionRect.posY += m_direction * m_speed * dt;
	checkBoreder();
}

void Platform::checkBoreder() {
	if (m_moveDirection == MoveDirection::P_HORIZONTAL) {
		if (collisionRect.posX + 100.0f >= m_end[0] && m_direction == 1.0f) {
			m_direction = -1.0f;
			collisionRect.posX = m_end[0] - 100.0f;
		}

		if (collisionRect.posX + 100.0f <= m_start[0] && m_direction == -1.0f) {
			m_direction = 1.0f;
			collisionRect.posX = m_start[0] - 100.0f;
		}

	}else {
		if (collisionRect.posY + 25.0f >= m_end[1] && m_direction == 1.0f) {
			m_direction = -1.0f;
			collisionRect.posY = m_end[1] - 25.0f;
		}

		if (collisionRect.posY + 25.0f <= m_start[1] && m_direction == -1.0f) {
			m_direction = 1.0f;
			collisionRect.posY = m_start[1] - 25.0f;
		}
	}
}

const CollisionRect& Platform::getCollisionRect() {
	return collisionRect;
}

Rect Platform::getRect() {
	return { collisionRect.posX, collisionRect.posY, collisionRect.width, collisionRect.height, collisionRect.hasCollision };
}