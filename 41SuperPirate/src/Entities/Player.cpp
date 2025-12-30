#include <iostream>
#include <engine/input/Keyboard.h>

#include "Player.h"

Player::Player(Cell& cell, CollisionRect& collisionRect, Camera& camera, const std::vector<CollisionRect>& collisionRects, float elpasedTime, int framecount) :
	SpriteEntity(cell, elpasedTime, framecount), 
	collisionRect(collisionRect),
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
	m_collideTop(false),
	m_onWall(false),
	m_wasCollideLeft(false),
	m_wasCollideRight(false),
	m_wantJump(false),
	m_waitForCollideBottom(false),
	m_wallBounceLeft(false),
	m_wallBounceRight(false),
	m_movingSpeed(0.0f),
	m_platformIndex(-1),
	m_sizeX(48.0f),
	m_sizeY(56.0f) {

	m_inputVector.set(0.0f, 0.0f);
	m_direction.set(0.0f, 0.0f);
	m_initialX = cell.posX;
	m_initialY = cell.posY;

	collisionRect.isPlayer = true;
}

Player::~Player() {

}

void Player::update(float dt) {
	collisionRect.previousRect = getRect();
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

	std::for_each(collisionRects.begin(), collisionRects.end(), [](const CollisionRect& rect) { rect.hasCollision = false; });
	
	if (move) {
		Vector2f inputVector = Vector2f::Normalize(m_inputVector);		
		m_direction[0] = (m_wallBounceLeft || m_wallBounceRight) ? m_direction[0] : inputVector[0];
		cell.posX += m_direction[0] * dt * m_movingSpeed;
		collisionRect.posX += m_direction[0] * dt * m_movingSpeed;
		collision(collisionRect, collisionRect.previousRect, CollisionAxis::HORIZONTAL);

		m_wallBounceLeft = false;
		m_wallBounceRight = false;
	}

	if (!m_collideBottom && (m_collideRight || m_collideLeft) && !m_jump && !m_onWall) {
		m_direction[1] = 0.0f;
		cell.posY += m_gravity * 0.1f * dt;	
		collisionRect.posY += m_gravity * 0.1f * dt;
	}else {
		m_direction[1] += m_gravity * 0.5f * dt;
		cell.posY += m_direction[1] * dt;
		collisionRect.posY += m_direction[1] * dt;
		m_direction[1] += m_gravity * 0.5f * dt;
		m_onWall = m_collideRight || m_collideLeft;
	}

	collision(collisionRect, collisionRect.previousRect, CollisionAxis::VERTICAL);

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
	platformMove();

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

	collisionRect = { cell.posX, cell.posY - 56.0f, m_sizeX, m_sizeY, false, true };
}

void Player::collision(const Rect& playerRect, const Rect& previousRect, CollisionAxis collisionAxis) {
	
	for(const CollisionRect& rect : collisionRects) {
		//rect.hasCollision = collisionAxis == CollisionAxis::HORIZONTAL ? false : rect.hasCollision;
		if (rect.isPlayer)
			continue;

		if(SpriteEntity::HasCollision(rect.posX, rect.posY, rect.posX + rect.width, rect.posY + rect.height, playerRect.posX, playerRect.posY, playerRect.posX + playerRect.width, playerRect.posY + playerRect.height)) {
			rect.hasCollision = true;
			if(collisionAxis == CollisionAxis::HORIZONTAL){
				if(playerRect.posX <= rect.posX + rect.width && previousRect.posX >= rect.previousRect.posX + rect.previousRect.width)
					cell.posX = rect.posX + rect.width;

				if(playerRect.posX + playerRect.width >= rect.posX && previousRect.posX + previousRect.width <= rect.previousRect.posX)
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
	Rect bottomRect = { cell.posX + 2.0f, cell.posY, 44.0f, 2.0f };
	Rect rightRect = { cell.posX + 48.0f, cell.posY - 42.0f , 2.0f, 28.0f };
	Rect leftRect = { cell.posX - 2.0f, cell.posY - 42.0f , 2.0f, 28.0f };
	Rect topRect = { cell.posX + 2.0f, cell.posY - 58.0f, 44.0f, 2.0f };
	
	int indexBottom, indexRight, indexLeft, indexTop;
	m_collideBottom = collideList(collisionRects, bottomRect, indexBottom);
	m_collideRight = collideList(collisionRects, rightRect, indexRight);
	m_collideLeft = collideList(collisionRects, leftRect, indexLeft);
	m_collideTop = collideList(collisionRects, topRect, indexTop);

	m_platformIndex = -1;
	for (size_t i = 0; i < collisionRects.size(); i++) {
		if (collisionRects[i].isMoving && SpriteEntity::HasCollision(collisionRects[i].posX, collisionRects[i].posY, collisionRects[i].posX + collisionRects[i].width, collisionRects[i].posY + collisionRects[i].height, bottomRect.posX, bottomRect.posY, bottomRect.posX + bottomRect.width, bottomRect.posY + bottomRect.height)) {
			m_platformIndex = i;
			if (m_collideBottom && indexBottom != m_platformIndex)
				m_platformIndex = -1;

			if (m_collideTop && indexTop != m_platformIndex) {
				float delta = collisionRect.posY - collisionRect.previousRect.posY;
				m_sizeY -= delta;
			}
		}
	}
}

void Player::platformMove() {
	
	if (m_platformIndex >= 0) {
		const CollisionRect& collisionRect = collisionRects[m_platformIndex];
		if (collisionRect.posX != collisionRect.previousRect.posX)
			cell.posX += collisionRect.posX - collisionRect.previousRect.posX;
		else
			cell.posY = collisionRect.posY;		
	}
}

const Rect Player::getBottomRect() {
	return { cell.posX + 2.0f, cell.posY, 44.0f, 2.0f };
}

const Rect Player::getLeftRect() {
	return { cell.posX - 2.0f, cell.posY - 42.0f , 2.0f, 28.0f };
}

const Rect Player::getRightRect() {
	return { cell.posX + 48.0f, cell.posY - 42.0f , 2.0f, 28.0f };
}

const Rect Player::getTopRect() {
	return { cell.posX + 2.0f, cell.posY - 58.0f, 44.0f, 2.0f };
}

bool Player::collideList(const std::vector<CollisionRect>& collisionRects, const Rect& _rect, int& index) {
	index = -1;
	for (size_t i = 0; i < collisionRects.size(); i++) {
		if (collisionRects[i].isPlayer)
			continue;

		if (SpriteEntity::HasCollision(collisionRects[i].posX, collisionRects[i].posY, collisionRects[i].posX + collisionRects[i].width, collisionRects[i].posY + collisionRects[i].height, _rect.posX, _rect.posY, _rect.posX + _rect.width, _rect.posY + _rect.height)) {
			collisionRects[i].hasCollision = true;
			index = i;
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

const Rect& Player::getRect() {
	return collisionRect;
}