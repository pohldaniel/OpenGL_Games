#include "Character.h"

Character::Character(Cell& cell) : SpriteEntity(cell) {

}

Character::~Character() {

}

void Character::update(float dt) {
	bool move = false;

	if (!move)
		return;
	updateAnimation(dt);
}