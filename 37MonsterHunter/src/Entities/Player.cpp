#include <engine/input/Keyboard.h>

#include "Player.h"

Player::Player(Cell& cell, Camera& camera, const std::vector<Rect>& collisionRects) : SpriteEntity(cell), camera(camera), collisionRects(collisionRects) {
	
}

Player::~Player() {

}

bool Player::hasCollision(float r1_l, float r1_t, float r1_r, float r1_b, float r2_l, float r2_t, float r2_r, float r2_b) {
	return (r2_b > r1_t) && (r2_t < r1_b) && (r1_l < r2_r) && (r2_l < r1_r);
}

void Player::update(float dt) {

	Keyboard &keyboard = Keyboard::instance();
	m_direction.set(0.0f, 0.0f);
	bool move = false;

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		m_direction += Vector2f(-1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		m_direction += Vector2f(1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		m_direction += Vector2f(0.0f, 1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		m_direction += Vector2f(0.0f, -1.0f);
		move |= true;
	}

	if (move) {
		m_direction.normalize();
		cell.posX += m_direction[0] * dt * m_movingSpeed;
		Rect playerRect = { cell.posX + 32.0f, cell.posY - (128.0f - 30.0f) , 128.0f - 64.0f, 128.0f - 60.0f };
		for (const Rect& rect : collisionRects) {
			if (hasCollision(rect.posX, rect.posY, rect.posX + rect.width, rect.posY + rect.height, playerRect.posX, playerRect.posY, playerRect.posX + playerRect.width, playerRect.posY + playerRect.height)) {

				if (m_direction[0] > 0) {
					cell.posX = rect.posX - (playerRect.width + 32.0f);
				}

				if (m_direction[0] < 0) {
					cell.posX = rect.posX + rect.width - 32.0f;
				}
			}
		}

		cell.posY -= m_direction[1] * dt * m_movingSpeed;
		playerRect = { cell.posX + 32.0f, cell.posY - (128.0f - 30.0f) , 128.0f - 64.0f, 128.0f - 60.0f };
		for (const Rect& rect : collisionRects) {
			if (hasCollision(rect.posX, rect.posY, rect.posX + rect.width, rect.posY + rect.height, playerRect.posX, playerRect.posY, playerRect.posX + playerRect.width, playerRect.posY + playerRect.height)) {
				if (m_direction[1] < 0) {
					cell.posY = rect.posY + 30.0f;
				}

				if (m_direction[1] > 0) {
					cell.posY = rect.posY + rect.height + 98.0f;
				}

			}
		}

		cell.centerX = cell.posX + 64.0f;
		cell.centerY = cell.posY - 64.0f;
		
	}
	camera.setPosition(cell.centerX - m_viewWidth * 0.5f, m_mapHeight - cell.centerY - 0.5f * m_viewHeight, 0.0f);

	updateAnimation(dt);
}



void Player::setMovingSpeed(float movingSpeed) {
	m_movingSpeed = movingSpeed;
}

void Player::setViewWidth(float viewWidth) {
	m_viewWidth = viewWidth;
}

void Player::setViewHeight(float viewHeight) {
	m_viewHeight = viewHeight;
}

void Player::setMapHeight(float mapHeight) {
	m_mapHeight = mapHeight;
}