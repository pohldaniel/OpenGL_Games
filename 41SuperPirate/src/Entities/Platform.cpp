#include "Platform.h"

Platform::Platform(Rect& collisonRect, MoveDirection moveDirection, const Vector2f& start, const Vector2f& end) :
	EntityNew(), 
	rect(collisonRect),
	m_moveDirection(moveDirection),
    m_start(start),
	m_end(end){

}

Platform::~Platform() {

}

void Platform::update(float dt) {

}