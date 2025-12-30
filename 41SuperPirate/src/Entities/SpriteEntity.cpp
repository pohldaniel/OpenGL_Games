#include "SpriteEntity.h"

SpriteEntity::SpriteEntity(Cell& cell, float elpasedTime, int framecount) : 
	EntityNew(),
	cell(cell),
	m_startFrame(cell.tileID), 
	m_elapsedTime(elpasedTime), 
	m_frameCount(framecount), 
	m_viewDirection(ViewDirection::RIGHT),
    m_previousRect(getRect()){

}

SpriteEntity& SpriteEntity::operator=(const SpriteEntity& rhs) {
	EntityNew::operator=(rhs);
	cell = rhs.cell;
	m_startFrame = rhs.m_startFrame;
	m_frameCount = rhs.m_frameCount;
	m_elapsedTime = rhs.m_elapsedTime;
	m_viewDirection = rhs.m_viewDirection;
	m_previousRect = rhs.m_previousRect;
	return *this;
}

SpriteEntity& SpriteEntity::operator=(SpriteEntity&& rhs) noexcept {
	EntityNew::operator=(rhs);
	cell = rhs.cell;
	m_startFrame = rhs.m_startFrame;
	m_frameCount = rhs.m_frameCount;
	m_elapsedTime = rhs.m_elapsedTime;
	m_viewDirection = rhs.m_viewDirection;
	m_previousRect = rhs.m_previousRect;
	return *this;
}

SpriteEntity::SpriteEntity(SpriteEntity const& rhs) : EntityNew(), cell(rhs.cell){
	m_startFrame = rhs.m_startFrame;
	m_frameCount = rhs.m_frameCount;
	m_elapsedTime = rhs.m_elapsedTime;
	m_viewDirection = rhs.m_viewDirection;
	m_previousRect = rhs.m_previousRect;
}

SpriteEntity::SpriteEntity(SpriteEntity&& rhs)  noexcept : EntityNew(), cell(rhs.cell) {
	m_startFrame = rhs.m_startFrame;
	m_frameCount = rhs.m_frameCount;
	m_elapsedTime = rhs.m_elapsedTime;
	m_viewDirection = rhs.m_viewDirection;
	m_previousRect = rhs.m_previousRect;
}

SpriteEntity::~SpriteEntity() {

}

const Cell& SpriteEntity::getCell() const {
	return cell;
}

void SpriteEntity::resetAnimation() {
	cell.tileID = m_startFrame;
	m_elapsedTime = 1.0f;
}

void SpriteEntity::updateAnimation(float dt) {
	int offset = m_startFrame;

	m_elapsedTime += 6.0f * dt;
	cell.tileID = offset + static_cast<int>(std::floor(m_elapsedTime));
	if (cell.tileID - offset > m_frameCount - 1) {
		cell.tileID = offset;
		m_elapsedTime -= static_cast<float>(m_frameCount);
	}

	cell.flipped = m_viewDirection == ViewDirection::LEFT;
}

bool SpriteEntity::HasCollision(float r1_l, float r1_t, float r1_r, float r1_b, float r2_l, float r2_t, float r2_r, float r2_b) {
	return (r2_b > r1_t) && (r2_t < r1_b) && (r1_l < r2_r) && (r2_l < r1_r);
}

Rect SpriteEntity::getRect() {
	return { cell.posX, cell.posY - cell.height, cell.width, cell.height, false };
}