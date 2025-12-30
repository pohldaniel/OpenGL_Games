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
	m_wasCollideRight(false),
	m_wantJump(false),
	m_waitForCollideBottom(false),
	m_wallBounceLeft(false),
	m_wallBounceRight(false),
	m_movingSpeed(0.0f){

	m_inputVector.set(0.0f, 0.0f);
	m_direction.set(0.0f, 0.0f);
	m_initialX = cell.posX;
	m_initialY = cell.posY;
}

Player::~Player() {

}

void Player::update(float dt) {
	m_previousRect = getRect();
	m_wallJumpTimer.update(dt);

	bool isAcivated = m_wallJumpTimer.isActivated();

	m_wasCollideLeft = isAcivated || m_collideLeft;
	m_wasCollideRight = isAcivated || m_collideRight;
	Keyboard& keyboard = Keyboard::instance();
	m_inputVector.set(0.0f, 0.0f);

	bool move = false;	
	if (m_wallBounceLeft || m_wallBounceRight) {
		if(m_wallBounceLeft)
			m_direction += Vector2f(-100.0f, 0.0f);

		if (m_wallBounceRight)
			m_direction += Vector2f(100.0f, 0.0f);

		move = true;
	}

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
		m_direction[0] = (m_wallBounceLeft || m_wallBounceRight) ? m_direction[0] : inputVector[0];
		cell.posX += m_direction[0] * dt * m_movingSpeed;
		Rect playerRect = getRect();
		collision(playerRect, m_previousRect, CollisionAxis::HORIZONTAL);	

		m_wallBounceLeft = false;
		m_wallBounceRight = false;
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

	if (m_jump && m_collideBottom) {
		m_wantJump = true;
	}

	/*if (m_jump && m_wasCollideLeft && !m_collideBottom) {
		m_wallBounceRight = true;
	}

	if (m_jump && m_wasCollideRight && !m_collideBottom) {
		m_wallBounceLeft = true;
	}*/

	if (m_jump && (m_wasCollideLeft || m_wasCollideRight) && !m_waitForCollideBottom) {
		m_wantJump = true;
		m_waitForCollideBottom = true;
	}

	m_waitForCollideBottom = !isAcivated;

	checkContact();

	if (((m_wasCollideLeft && !m_collideLeft) || (m_wasCollideRight && !m_collideRight)) && !m_collideBottom) {
		if(m_inputVector[0] != 0.0f)
			m_wallJumpTimer.start(400u, false);
	}

	if (m_collideBottom && m_direction[1] > 0.0f) {
		m_waitForCollideBottom = false;
	}
	
	if (m_wantJump) {
		m_direction[1] = -m_jumpHeight;
		m_wantJump = false;
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
	Rect bottomRect = { cell.posX + 2.0f, cell.posY, 46.0f, 2.0f };	
	Rect rightRect = { cell.posX + 48.0f, cell.posY - 42.0f , 2.0f, 28.0f };
	Rect leftRect = { cell.posX - 2.0f, cell.posY - 42.0f , 2.0f, 28.0f };

	m_collideBottom = collideList(collisionRects, bottomRect);
	m_collideRight = collideList(collisionRects, rightRect);
	m_collideLeft = collideList(collisionRects, leftRect);
}

const Rect Player::getBottomRect() {
	return { cell.posX + 2.0f, cell.posY, 46.0f, 2.0f };
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

void Player::setMovingSpeed(float movingSpeed) {
	m_movingSpeed = movingSpeed;
}

void Player::reset() {
	cell.posX = m_initialX;
	cell.posY = m_initialY;
}

Rect Player::getRect() {
	return { cell.posX, cell.posY - 56.0f, 48.0f, 56.0f, false };
}