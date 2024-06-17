#include <iostream>
#include "SpriteEntity.h"

SpriteEntity::SpriteEntity(Cell& cell, float elpasedTime, int framecount) : cell(cell), m_startFrame(cell.currentFrame), m_elapsedTime(elpasedTime), m_frameCount(framecount) {

}

SpriteEntity::~SpriteEntity() {

}

/*void SpriteEntity::update(float dt) {
	
}*/

const Cell& SpriteEntity::getCell() {
	return cell;
}