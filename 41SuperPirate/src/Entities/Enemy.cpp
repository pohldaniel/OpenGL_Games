#include <iostream>
#include "Enemy.h"

Enemy::Enemy(Cell& cell, float elpasedTime, int framecount) : SpriteEntity(cell, elpasedTime, framecount) {
	m_direction.set(0.0f, 0.0f);
}

Enemy::~Enemy() {

}

void Enemy::update(float dt) {
	updateAnimation(dt);
}