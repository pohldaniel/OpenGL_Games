#include <iostream>
#include "Character.h"

Character::Character(Cell& cell) : SpriteEntity(cell) {

}

Character::~Character() {

}

void Character::update(float dt) {
	if (!cell.visibile) {
		cell.currentFrame = m_startFrame;
		return;
	}

	m_elapsedTime += 6.0f * dt;
	cell.currentFrame = m_startFrame + static_cast <int>(std::floor(m_elapsedTime));
	if (cell.currentFrame - m_startFrame > m_frameCount - 1) {
		cell.currentFrame = m_startFrame;
		m_elapsedTime -= static_cast <float>(m_frameCount);
	}
}