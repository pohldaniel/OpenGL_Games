#include "Enemy.h"

Enemy::Enemy(Cell& cell, float elpasedTime, int framecount) : SpriteEntity(cell, elpasedTime, framecount) {

}

Enemy::~Enemy() {

}

void Enemy::update(float dt) {
	updateAnimation(dt);
}