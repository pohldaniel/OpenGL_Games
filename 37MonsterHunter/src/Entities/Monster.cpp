#include "Monster.h"

Monster::Monster(Cell& cell) : SpriteEntity(cell) {
	m_direction.set(0.0f, 0.0f);
}

Monster::~Monster() {

}

void Monster::update(float dt) {

}