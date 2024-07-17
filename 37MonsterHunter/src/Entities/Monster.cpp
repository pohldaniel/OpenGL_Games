#include "Monster.h"

Monster::Monster(Cell& cell) : SpriteEntity(cell) {
	m_direction.set(0.0f, 0.0f);
}

Monster::~Monster() {

}

void Monster::update(float dt) {
	m_elapsedTime += 6.0f * dt;
	cell.currentFrame = m_startFrame + static_cast<int>(std::floor(m_elapsedTime));
	if (cell.currentFrame - m_startFrame > m_frameCount - 1) {
		cell.currentFrame = m_startFrame;
		m_elapsedTime -= static_cast<float>(m_frameCount);
	}
}