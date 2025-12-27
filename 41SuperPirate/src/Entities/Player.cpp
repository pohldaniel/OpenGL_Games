#include <iostream>
#include <engine/input/Keyboard.h>

#include "Player.h"

Player::Player(Cell& cell, Camera& camera, const std::vector<Rect>& collisionRects, float elpasedTime, int framecount) :
	SpriteEntity(cell, elpasedTime, framecount), 
	camera(camera), 
	collisionRects(collisionRects),
    m_mapHeight(0.0f),
    m_viewHeight(0.0f),
    m_viewWidth(0.0f),
	m_gravity(1300.0f){

	m_direction.set(0.0f, 0.0f);
	m_initialX = cell.posX;
	m_initialY = cell.posY;
}

Player::~Player() {

}

void Player::update(float dt) {
	m_previousRect = getRect();
	Keyboard& keyboard = Keyboard::instance();
	m_direction[0] = 0.0f;
	//m_direction[1] = 0.0f;
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

	if (keyboard.keyPressed(Keyboard::KEY_W) || keyboard.keyPressed(Keyboard::KEY_SPACE)) {
		//m_direction += Vector2f(0.0f, -1.0f);
		//move |= true;
		m_direction[1] = -200.0f;
	}

	/*if (keyboard.keyDown(Keyboard::KEY_S)) {
		m_direction += Vector2f(0.0f, 1.0f);
		move |= true;
	}*/

	std::for_each(collisionRects.begin(), collisionRects.end(), [](const Rect& rect) { rect.hasCollision = false; });
	if (move) {
		//m_direction.normalize();
		Vector2f direction = Vector2f::Normalize(m_direction);
		m_direction[0] = direction[0];

		cell.posX += m_direction[0] * dt * m_movingSpeed;		
		Rect playerRect = { cell.posX, cell.posY - 56.0f, 48.0f, 56.0f, false };
		collision(playerRect, m_previousRect, CollisionAxis::HORIZONTAL);	
	}
	m_direction[1] += m_gravity * 0.5f * dt;
	cell.posY += m_direction[1] * dt;
	m_direction[1] += m_gravity * 0.5f * dt;
	//cell.posY += m_direction[1] * dt * m_movingSpeed;
	Rect playerRect = { cell.posX, cell.posY - 56.0f, 48.0f, 56.0f, false };
	collision(playerRect, m_previousRect, CollisionAxis::VERTICAL);
	

	cell.centerX = cell.posX + 48.0f;
	cell.centerY = cell.posY - 56.0f;

	//camera.setPosition(cell.centerX - m_viewWidth * 0.5f, m_mapHeight - cell.centerY - 0.5f * m_viewHeight, 0.0f);
	updateAnimation(dt);
}

void Player::collision(const Rect& playerRect, const Rect& previousRect, CollisionAxis collisionAxis) {
	
	for (const Rect& rect : collisionRects) {
		
		//rect.hasCollision = collisionAxis == CollisionAxis::HORIZONTAL ? false : rect.hasCollision;

		if (SpriteEntity::HasCollision(rect.posX, rect.posY, rect.posX + rect.width, rect.posY + rect.height, playerRect.posX, playerRect.posY, playerRect.posX + playerRect.width, playerRect.posY + playerRect.height)) {
			rect.hasCollision = true;
			if (collisionAxis == CollisionAxis::HORIZONTAL) {
				if (playerRect.posX <= rect.posX + rect.width && previousRect.posX >= rect.posX + rect.width)
					cell.posX = rect.posX + rect.width;

				if (playerRect.posX + playerRect.width >= rect.posX && previousRect.posX + previousRect.width <= rect.posX)
					cell.posX = rect.posX - 48.0f;
			}else {
				if (playerRect.posY <= rect.posY + rect.height && previousRect.posY >= rect.posY + rect.height)
					cell.posY = rect.posY + rect.height + 56.0f /*+ 0.1f*/;

				if (playerRect.posY + playerRect.height >= rect.posY && previousRect.posY + previousRect.height <= rect.posY)
					cell.posY = rect.posY /* - 0.1f*/;

				m_direction[1] = 0.0f;
			}
		}
	}
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