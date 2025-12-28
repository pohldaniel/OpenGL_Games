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
	m_gravity(1300.0f),
	m_jump(false),
	m_jumpHeight(900.0f),
	m_collideBottom(false),
	m_collideLeft(false),
	m_collideRight(false),
	m_onWall(false),
	m_wasCollideLeft(false),
	m_wasCollideRight(false){

	m_inputVector.set(0.0f, 0.0f);
	m_direction.set(0.0f, 0.0f);
	m_initialX = cell.posX;
	m_initialY = cell.posY;

	m_wallJumpTimer.setOnTimerEnd([&wallJumpTimer = m_wallJumpTimer, &wasCollideLeft = m_wasCollideLeft, &wasCollideRight = m_wasCollideRight] {
		//wallJumpTimer.reset();
		wallJumpTimer.stop();
		wasCollideLeft = false;
		wasCollideRight = false;
		//wallJumpTimer.setStartOnce(false);
	});
}

Player::~Player() {

}

void Player::update(float dt) {
	m_previousRect = getRect();

	m_wasCollideLeft = m_wallJumpTimer.isActivated() || m_collideLeft;
	m_wasCollideRight = m_wallJumpTimer.isActivated() || m_collideRight;
	m_wallJumpTimer.update(dt);

	Keyboard& keyboard = Keyboard::instance();
	m_inputVector.set(0.0f, 0.0f);
	bool move = false;
		
	if (keyboard.keyDown(Keyboard::KEY_A)) {
		m_inputVector += Vector2f(-1.0f, 0.0f);
		m_viewDirection = ViewDirection::LEFT;
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		m_inputVector += Vector2f(1.0f, 0.0f);
		m_viewDirection = ViewDirection::RIGHT;
		move |= true;
	}
	
	if (keyboard.keyPressed(Keyboard::KEY_W)) {
		m_jump = true;
	}

	std::for_each(collisionRects.begin(), collisionRects.end(), [](const Rect& rect) { rect.hasCollision = false; });
	

	if (move) {
		Vector2f inputVector = Vector2f::Normalize(m_inputVector);		
		m_direction[0] = inputVector[0];
		cell.posX += m_direction[0] * dt * m_movingSpeed;
		Rect playerRect = getRect();
		collision(playerRect, m_previousRect, CollisionAxis::HORIZONTAL);	
	}

	if (!m_collideBottom && (m_collideRight || m_collideLeft) && !m_jump && !m_onWall) {
		m_direction[1] = 0.0f;
		cell.posY += m_gravity * 0.1f * dt;			
	}else {
		m_direction[1] += m_gravity * 0.5f * dt;
		cell.posY += m_direction[1] * dt;
		m_direction[1] += m_gravity * 0.5f * dt;
		m_onWall = m_collideRight || m_collideLeft;
	}
	Rect playerRect = getRect();
	collision(playerRect, m_previousRect, CollisionAxis::VERTICAL);

	/*if (m_jump) {
		if(m_collideBottom)
			m_direction[1] = -m_jumpHeight;
		else if (m_collideRight || m_collideLeft) {
			m_wallJumpTimer.start(200u, false);
			m_direction[1] = -m_jumpHeight;
			cell.posX = m_collideLeft ? cell.posX + 1.0f : cell.posX -1.0f;
		}
	}*/

	if (m_jump && (m_collideBottom || m_collideRight || m_collideLeft)) {
		m_direction[1] = -m_jumpHeight;
	}
	
	checkContact();
	if (((m_wasCollideLeft && !m_collideLeft) || (m_wasCollideRight && !m_collideRight)) && !m_collideBottom) {
		m_wallJumpTimer.start(250u, false);
	}

	if ((m_wasCollideLeft || m_wasCollideRight) && m_jump) {
		m_direction[1] = -m_jumpHeight;
	}

	if (m_jump)
		m_jump = false;

	updateAnimation(dt);
}

void Player::collision(const Rect& playerRect, const Rect& previousRect, CollisionAxis collisionAxis) {
	
	for(const Rect& rect : collisionRects) {
		
		//rect.hasCollision = collisionAxis == CollisionAxis::HORIZONTAL ? false : rect.hasCollision;

		if(SpriteEntity::HasCollision(rect.posX, rect.posY, rect.posX + rect.width, rect.posY + rect.height, playerRect.posX, playerRect.posY, playerRect.posX + playerRect.width, playerRect.posY + playerRect.height)) {
			rect.hasCollision = true;
			if(collisionAxis == CollisionAxis::HORIZONTAL){
				if(playerRect.posX <= rect.posX + rect.width && previousRect.posX >= rect.posX + rect.width)
					cell.posX = rect.posX + rect.width;

				if(playerRect.posX + playerRect.width >= rect.posX && previousRect.posX + previousRect.width <= rect.posX)
					cell.posX = rect.posX - 48.0f;
			}else{
				if(playerRect.posY <= rect.posY + rect.height && previousRect.posY >= rect.posY + rect.height)
					cell.posY = rect.posY + rect.height + 56.0f /*+ 0.1f*/;

				if(playerRect.posY + playerRect.height >= rect.posY && previousRect.posY + previousRect.height <= rect.posY)
					cell.posY = rect.posY /* - 0.1f*/;

				m_direction[1] = 0.0f;
			}
		}
	}
}

void Player::checkContact() {
	Rect bottomRect = { cell.posX, cell.posY, 48.0f, 2.0f };	
	Rect rightRect = { cell.posX + 48.0f, cell.posY - 42.0f , 2.0f, 28.0f };
	Rect leftRect = { cell.posX - 2.0f, cell.posY - 42.0f , 2.0f, 28.0f };

	m_collideBottom = collideList(collisionRects, bottomRect);
	m_collideRight = collideList(collisionRects, rightRect);
	m_collideLeft = collideList(collisionRects, leftRect);
}

const Rect Player::getBottomRect() {
	return { cell.posX, cell.posY, 48.0f, 2.0f };
}

const Rect Player::getLeftRect() {
	return { cell.posX - 2.0f, cell.posY - 42.0f , 2.0f, 28.0f };
}

const Rect Player::getRightRect() {
	return { cell.posX + 48.0f, cell.posY - 42.0f , 2.0f, 28.0f };
}

bool Player::collideList(const std::vector<Rect>& collisionRects, const Rect& _rect) {	
	for (const Rect& rect : collisionRects) {
		if (SpriteEntity::HasCollision(rect.posX, rect.posY, rect.posX + rect.width, rect.posY + rect.height, _rect.posX, _rect.posY, _rect.posX + _rect.width, _rect.posY + _rect.height)) {
			rect.hasCollision = true;
			return true;
		}			
	}
	return false;
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