#include <iostream>
#include "SpriteEntity.h"

SpriteEntity::SpriteEntity(Cell& cell, float elpasedTime, int framecount) : cell(cell), m_startFrame(cell.currentFrame), m_elapsedTime(elpasedTime), m_frameCount(framecount) {
	m_direction.set(0.0f, 0.0f);
	m_viewDirection = ViewDirection::NONE;
	m_lastViewDirection = m_viewDirection;
}

SpriteEntity::~SpriteEntity() {

}

const Cell& SpriteEntity::getCell() const {
	return cell;
}

void SpriteEntity::setDirection(const Vector2f& direction) {
	m_direction = direction;
	cell.currentFrame = m_startFrame + getFrameOffset(getViewDirection());
	updateLastViewDirection();
}

void SpriteEntity::updateLastViewDirection() {
	if (m_viewDirection != ViewDirection::NONE) {
		m_lastViewDirection = m_viewDirection;
	}
}

const ViewDirection& SpriteEntity::getViewDirection(){
	updateLastViewDirection();
	if (m_direction[0] != 0.0f || m_direction[1] != 0.0f) {
		if (m_direction[0] != 0.0f)
			m_viewDirection = m_direction[0] > 0.0f ? ViewDirection::RIGHT : ViewDirection::LEFT;
		if (m_direction[1] != 0.0f)
			m_viewDirection = m_direction[1] > 0.0f ? ViewDirection::UP : ViewDirection::DOWN;
	}else {
		m_viewDirection = ViewDirection::NONE;
	}

	return m_viewDirection;
}

int SpriteEntity::getFrameOffset(ViewDirection viewDirection) {
	switch (viewDirection){
		case LEFT:
			return 4;
		case RIGHT:
			return 8;
		case DOWN:
			return 0;
		case UP:
			return 12;
		case NONE:
			return 0;
		default:
			return 0;
	}
}

void SpriteEntity::updateAnimation(float dt) {
	const ViewDirection& direction = getViewDirection();
	if (direction == ViewDirection::NONE) {
		cell.currentFrame = m_startFrame + getFrameOffset(m_lastViewDirection);
		m_elapsedTime = 1.0f;
		return;
	}

	int offset = m_startFrame + getFrameOffset(m_viewDirection);

	m_elapsedTime += 6.0f * dt;
	cell.currentFrame = offset + static_cast <int>(std::floor(m_elapsedTime));
	if (cell.currentFrame - offset > m_frameCount - 1) {
		cell.currentFrame = offset;
		m_elapsedTime -= static_cast <float>(m_frameCount);
	}
}

Vector2f SpriteEntity::GetDirection(std::string direction) {
	if (direction == "down")
		return Vector2f(0.0f, -1.0f);
	else if (direction == "up")
		return Vector2f(0.0f, 1.0f);
	else if (direction == "left")
		return Vector2f(-1.0f, 0.0f);
	else if (direction == "right")
		return Vector2f(1.0f, 0.0f);

	return Vector2f(0.0f, 0.0f);
}