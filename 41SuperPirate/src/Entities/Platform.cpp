#include <iostream>
#include "Platform.h"

Platform::Platform(Rect& collisonRect, MoveDirection moveDirection, float speed, const Vector2f& start, const Vector2f& end) :
	EntityNew(), 
	rect(collisonRect),
	m_previousRect(collisonRect),
	m_moveDirection(moveDirection),
	m_speed(speed),
    m_start(start),
	m_end(end),
	m_direction(1.0f){
	//moveDirection == MoveDirection::P_HORIZONTAL ? m_direction[0] = 1.0f : m_direction[1] = 1.0f;	
}

Platform::~Platform() {
	
}

void Platform::update(float dt) {
	m_previousRect = getRect();
	m_moveDirection == MoveDirection::P_HORIZONTAL ? rect.posX += m_direction * m_speed * dt : rect.posY += m_direction * m_speed * dt;
	checkBoreder();
}

void Platform::checkBoreder() {
	if (m_moveDirection == MoveDirection::P_HORIZONTAL) {
		if (rect.posX + 100.0f >= m_end[0] && m_direction == 1.0f) {
			m_direction = -1.0f;
			rect.posX = m_end[0] - 100.0f;
		}

		if (rect.posX + 100.0f <= m_start[0] && m_direction == -1.0f) {
			m_direction = 1.0f;
			rect.posX = m_start[0] - 100.0f;
		}

	}else {
		if (rect.posY + 25.0f >= m_end[1] && m_direction == 1.0f) {
			m_direction = -1.0f;
			rect.posY = m_end[1] - 25.0f;
		}

		if (rect.posY + 25.0f <= m_start[1] && m_direction == -1.0f) {
			m_direction = 1.0f;
			rect.posY = m_start[1] - 25.0f;
		}
	}
}

const Rect& Platform::getRect() {
	return rect;
}

const Rect& Platform::getPreviousRect() {
	return m_previousRect;
}