#include <iostream>
#include <engine/input/Keyboard.h>

#include "Player.h"

Player::Player(Cell& cell, Camera& camera, const std::vector<Rect>& collisionRects, float elpasedTime, int framecount) :
	SpriteEntity(cell, elpasedTime, framecount), 
	camera(camera), 
	collisionRects(collisionRects),
    m_mapHeight(0.0f),
    m_viewHeight(0.0f),
    m_viewWidth(0.0f){

	m_direction.set(0.0f, 0.0f);
	m_initialX = cell.posX;
	m_initialY = cell.posY;
}

Player::~Player() {

}

void Player::update(float dt) {
	m_previousRect = getRect();
	Keyboard& keyboard = Keyboard::instance();
	m_direction.set(0.0f, 0.0f);
	bool move = false;

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		m_direction += Vector2f(-1.0f, 0.0f);
		m_viewDirection = ViewDirection::LEFT;
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		m_direction += Vector2f(1.0f, 0.0f);
		m_viewDirection = ViewDirection::RIGHT;
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
		Rect playerRect = { cell.posX, cell.posY - 56.0f, 48.0f, 56.0f, false };
		for (const Rect& rect : collisionRects) {
			rect.hasCollision = false;
			if (SpriteEntity::HasCollision(rect.posX, rect.posY, rect.posX + rect.width, rect.posY + rect.height, playerRect.posX, playerRect.posY, playerRect.posX + playerRect.width, playerRect.posY + playerRect.height)) {
				rect.hasCollision = true;
				if (playerRect.posX < rect.posX + rect.width /* && m_previousRect.posX > rect.posX + rect.width*/)
					cell.posX = rect.posX + rect.width;

				if (playerRect.posX + playerRect.width > rect.posX /*&& m_previousRect.posX + m_previousRect.width < rect.posX*/)
					cell.posX = rect.posX - 48.0f;
			}
		}

		

		cell.posY -= m_direction[1] * dt * m_movingSpeed;

		cell.centerX = cell.posX + 48.0f;
		cell.centerY = cell.posY - 56.0f;
	}

	

	//camera.setPosition(cell.centerX - m_viewWidth * 0.5f, m_mapHeight - cell.centerY - 0.5f * m_viewHeight, 0.0f);
	updateAnimation(dt);
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

void Player::reset() {
	cell.posX = m_initialX;
	cell.posY = m_initialY;
}

Rect Player::getRect() {
	return { cell.posX, cell.posY - 56.0f, 48.0f, 56.0f, false };
}