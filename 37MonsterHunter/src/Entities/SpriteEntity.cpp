#include <iostream>
#include "SpriteEntity.h"

SpriteEntity::SpriteEntity(Cell& cell, float elpasedTime, int framecount) : cell(cell), m_startFrame(cell.currentFrame), m_elapsedTime(elpasedTime), m_frameCount(framecount), m_movingSpeed(0.0f){
	m_viewDirection = ViewDirection::NONE;
	m_lastViewDirection = m_viewDirection;
}

SpriteEntity::~SpriteEntity() {

}

const Cell& SpriteEntity::getCell() const {
	return cell;
}

void SpriteEntity::setViewDirection(ViewDirection direction) {
	m_viewDirection = direction;
	m_lastViewDirection = m_viewDirection;
	m_direction = GetDirection(m_viewDirection);
	cell.currentFrame = m_startFrame + getFrameOffset(m_viewDirection);
}

void SpriteEntity::updateLastViewDirection() {
	if (m_viewDirection != ViewDirection::NONE) {
		m_lastViewDirection = m_viewDirection;
	}
}

const ViewDirection& SpriteEntity::getViewDirection() {
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

void SpriteEntity::resetAnimation() {
	cell.currentFrame = m_startFrame + getFrameOffset(m_lastViewDirection);
	m_elapsedTime = 1.0f;
}

void SpriteEntity::updateAnimation(float dt) {
	const ViewDirection& direction = getViewDirection();
	if (direction == ViewDirection::NONE) {
		resetAnimation();
		return;
	}

	int offset = m_startFrame + getFrameOffset(m_viewDirection);

	m_elapsedTime += 6.0f * dt;
	cell.currentFrame = offset + static_cast<int>(std::floor(m_elapsedTime));
	if (cell.currentFrame - offset > m_frameCount - 1) {
		cell.currentFrame = offset;
		m_elapsedTime -= static_cast<float>(m_frameCount);
	}
}

ViewDirection SpriteEntity::GetDirection(std::string direction) {
	if (direction == "down")
		return ViewDirection::DOWN;
	else if (direction == "up")
		return ViewDirection::UP;
	else if (direction == "left")
		return ViewDirection::LEFT;
	else if (direction == "right")
		return ViewDirection::RIGHT;

	return ViewDirection::NONE;
}

Vector2f SpriteEntity::GetDirection(ViewDirection direction) {
	if (direction == ViewDirection::DOWN)
		return Vector2f(0.0f, -1.0f);
	else if (direction == ViewDirection::UP)
		return Vector2f(0.0f, 1.0f);
	else if (direction == ViewDirection::LEFT)
		return Vector2f(-1.0f, 0.0f);
	else if (direction == ViewDirection::RIGHT)
		return Vector2f(1.0f, 0.0f);

	return Vector2f(0.0f, 0.0f);
}

const Vector2f& SpriteEntity::getDirection() const {
	return m_direction;
}

void SpriteEntity::setMovingSpeed(float movingSpeed) {
	m_movingSpeed = movingSpeed;
}