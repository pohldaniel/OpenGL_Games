#include <iostream>
#include "Platform.h"

Platform::Platform(CollisionRect& dynmaicRect, MoveDirection moveDirection, float speed, const Vector2f& start, const Vector2f& end) :
	EntityNew(), 
	dynmaicRect(dynmaicRect),
	m_moveDirection(moveDirection),
	m_speed(speed),
    m_start(start),
	m_end(end),
	m_direction(1.0f){
	dynmaicRect.isMoving = true;
}

Platform::~Platform() {
	
}

void Platform::update(float dt) {
	dynmaicRect.previousRect = { dynmaicRect.posX, dynmaicRect.posY, dynmaicRect.width, dynmaicRect.height, dynmaicRect.hasCollision };

	m_moveDirection == MoveDirection::P_HORIZONTAL ? dynmaicRect.posX += m_direction * m_speed * dt : dynmaicRect.posY += m_direction * m_speed * dt;
	checkBoreder();
}

void Platform::checkBoreder() {
	if (m_moveDirection == MoveDirection::P_HORIZONTAL) {
		if (dynmaicRect.posX + 100.0f >= m_end[0] && m_direction == 1.0f) {
			m_direction = -1.0f;
			dynmaicRect.posX = m_end[0] - 100.0f;
		}

		if (dynmaicRect.posX + 100.0f <= m_start[0] && m_direction == -1.0f) {
			m_direction = 1.0f;
			dynmaicRect.posX = m_start[0] - 100.0f;
		}

	}else {
		if (dynmaicRect.posY + 25.0f >= m_end[1] && m_direction == 1.0f) {
			m_direction = -1.0f;
			dynmaicRect.posY = m_end[1] - 25.0f;
		}

		if (dynmaicRect.posY + 25.0f <= m_start[1] && m_direction == -1.0f) {
			m_direction = 1.0f;
			dynmaicRect.posY = m_start[1] - 25.0f;
		}
	}
}

const CollisionRect& Platform::getDynamicRect() {
	return dynmaicRect;
}

Rect Platform::getRect() {
	return { dynmaicRect.posX, dynmaicRect.posY, dynmaicRect.width, dynmaicRect.height, dynmaicRect.hasCollision };
}