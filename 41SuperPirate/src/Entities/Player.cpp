#include "Player.h"

Player::Player(Cell& cell, Camera& camera, const std::vector<Rect>& collisionRects) : SpriteEntity(cell), camera(camera), collisionRects(collisionRects) {
	m_direction.set(0.0f, 0.0f);
}

Player::~Player() {

}