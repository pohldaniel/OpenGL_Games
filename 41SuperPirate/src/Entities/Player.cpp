#include <iostream>
#include <engine/input/Keyboard.h>

#include "Player.h"

Player::Player(Cell& cell, DynamicRect& collisionRect, Camera& camera, const std::vector<Rect>& staticRects, const std::vector<DynamicRect>& dynamicRects, float elpasedTime, int framecount) :
	SpriteEntity(cell, elpasedTime, framecount), 
	collisionRect(collisionRect),
	camera(camera), 
	staticRects(staticRects),
	dynamicRects(dynamicRects),
    m_mapHeight(0.0f),
    m_viewHeight(0.0f),
    m_viewWidth(0.0f),
	m_gravity(1300.0f),
	m_jumpPressed(false),
	m_blockJump(false),
	m_jumpHeight(900.0f),
	m_collideBottom(false),
	m_collideLeft(false),
	m_collideRight(false),
	m_collideTop(false),
	m_wasCollideBottom(false),
	m_wasCollideLeft(false),
	m_wasCollideRight(false),
	m_wantJump(false),
	m_waitForCollideBottom(false),
	m_wallBounceLeft(false),
	m_wallBounceRight(false),
	m_collideDynamic(false),
	m_wasCollideDynamic(false),
	m_blockWallSlide(false),
	m_onWall(false),
	m_movingSpeed(0.0f),
	m_platformIndex(-1),
	m_sizeX(48.0f),
	m_sizeY(56.0f),
	m_wantReset(false),
	m_upPressed(false),
	m_dashPressed(false){

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
	m_coyoteWallJumpTimer.update(dt);
	m_coyoteJumpTimer.update(dt);
	m_jumpBuffer.update(dt);
	m_dashTimer.update(dt);

	bool isAcivated = m_coyoteWallJumpTimer.isActivated();
	m_wasCollideBottom = m_coyoteJumpTimer.isActivated() || m_collideBottom;
	m_wasCollideLeft = isAcivated || m_collideLeft;
	m_wasCollideRight = isAcivated || m_collideRight;
	m_wasCollideDynamic = m_collideDynamic;

	Keyboard& keyboard = Keyboard::instance();
	m_inputVector.set(0.0f, 0.0f);

	bool move = false;	
	bool wantDash = false;

	float speed = m_movingSpeed;

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
		m_upPressed = true;
		m_jumpPressed = true;
		if (!m_collideBottom)
			m_jumpBuffer.start(200u, false);
	}

	if (keyboard.keyUp(Keyboard::KEY_W) && m_upPressed) {
		m_upPressed = false;
		//m_direction[1] = 0.0f;
	}

	if (!m_dashPressed && (keyboard.keyDown(Keyboard::KEY_LEFT) || keyboard.keyDown(Keyboard::KEY_RIGHT) || keyboard.keyDown(Keyboard::KEY_SPACE))) {
		m_dashPressed = true;
		m_dashTimer.startExclusive(100u, false, 400u);		
	}

	if (m_dashPressed && (keyboard.keyDown(Keyboard::KEY_LEFT) || keyboard.keyDown(Keyboard::KEY_RIGHT) || keyboard.keyDown(Keyboard::KEY_SPACE))) {
		m_dashPressed = false;
	}

	if (m_dashTimer.isActivated()) {
		m_inputVector.set(0.0f, 0.0f);
		if (m_viewDirection == ViewDirection::LEFT)
			m_inputVector = Vector2f(-1.0f, 0.0f);

		if (m_viewDirection == ViewDirection::RIGHT)
			m_inputVector = Vector2f(1.0f, 0.0f);
		
		wantDash = true;
		speed = m_movingSpeed * 4.0f;
	}

	if (m_wallBounceLeft || m_wallBounceRight) {
		if (m_wallBounceLeft)
			m_direction = Vector2f(-100.0f, 0.0f);

		if (m_wallBounceRight)
			m_direction = Vector2f(100.0f, 0.0f);

		move = true;
	}

	std::for_each(staticRects.begin(), staticRects.end(), [](const Rect& rect) { rect.hasCollision = false; });
	std::for_each(dynamicRects.begin(), dynamicRects.end(), [](const DynamicRect& rect) { rect.hasCollision = false; });

	int staticIndex = -1;
	int dynamicIndex = -1;

	if (move || wantDash) {
		Vector2f inputVector = Vector2f::Normalize(m_inputVector);		
		m_direction[0] = (m_wallBounceLeft || m_wallBounceRight) ? m_direction[0] : inputVector[0];
		
		cell.posX += m_direction[0] * dt * speed;
		collisionRect.posX += m_direction[0] * dt * speed;

		collision(collisionRect, collisionRect.previousRect, CollisionAxis::HORIZONTAL, staticIndex, dynamicIndex);

		m_wallBounceLeft = false;
		m_wallBounceRight = false;
	}

	if (wantDash) {
		collisionRect = { cell.posX, cell.posY - 56.0f, m_sizeX, m_sizeY, false, true };
		return;
	}

	if (!m_collideBottom && (m_collideRight || m_collideLeft) && m_jumpPressed && !m_blockJump) {
		m_wantJump = (m_inputVector[0] == 0.0f || (m_inputVector[0] < 0.0f || m_collideLeft) || (m_inputVector[0] > 0.0f || m_collideRight));
		m_blockJump = true;
	}
	

	if ((m_blockJump && m_jumpPressed) || ((!m_collideBottom && (m_collideRight || m_collideLeft) && !m_jumpPressed && !m_onWall && !m_blockWallSlide) && m_direction[1] >= 0.0f)) {
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
		
	collision(collisionRect, collisionRect.previousRect, CollisionAxis::VERTICAL, staticIndex, dynamicIndex);

	if (m_collideBottom) {
		bool activateJumpBuffer = m_jumpBuffer.isActivated();
		m_wantJump = m_jumpPressed || activateJumpBuffer;
		if (activateJumpBuffer)
			m_jumpBuffer.stop();
	}

	/*if (m_jumpPressed && m_wasCollideLeft && !m_collideBottom) {
		m_wallBounceRight = true;
	}

	if (m_jumpPressed && m_wasCollideRight && !m_collideBottom) {
		m_wallBounceLeft = true;
	}*/
	
	if (m_jumpPressed && m_wasCollideBottom) {
		m_wantJump = true;
		m_waitForCollideBottom = true;
		m_wasCollideBottom = false;
		m_coyoteJumpTimer.stop();
	}

	if (m_jumpPressed && (m_wasCollideLeft || m_wasCollideRight) && !m_waitForCollideBottom) {
		m_wantJump = true;
		m_waitForCollideBottom = true;		
	}

	m_waitForCollideBottom = !isAcivated;

	checkContact();
	platformMove();

	if (m_wasCollideDynamic && !m_collideDynamic) {
		m_blockWallSlide = true;
	}

	if (m_wasCollideBottom && !m_collideBottom) {
		m_coyoteJumpTimer.start(100u, false);
	}

	if (((m_wasCollideLeft && !m_collideLeft) || (m_wasCollideRight && !m_collideRight)) && !m_collideBottom) {
		if (m_inputVector[0] != 0.0f) {
			m_blockJump = false;
			m_coyoteWallJumpTimer.start(400u, false);
		}
	}

	if (m_collideBottom && m_direction[1] > 0.0f) {
		m_waitForCollideBottom = false;
		m_blockJump = false;
		m_blockWallSlide = false;
		m_direction[1] = 0.0f;
	}

	if (m_wantJump) {
		m_direction[1] = -m_jumpHeight;
		m_wantJump = false;
	}

	if (m_collideTop && m_direction[1] < 0.0f)
		m_direction[1] = 0.0f;

	if (m_jumpPressed)
		m_jumpPressed = false;
	
	updateAnimation(dt);

	if (m_wantReset) {
		m_wantReset = false;
		cell.posX = m_initialX;
		cell.posY = m_initialY;
	}
	collisionRect = { cell.posX, cell.posY - 56.0f, m_sizeX, m_sizeY, false, true };
}

void Player::collision(const Rect& playerRect, const Rect& previousRect, CollisionAxis collisionAxis, int& staticIndex, int& dynamicIndex) {

	for (size_t i = 0; i < staticRects.size(); i++) {
		if (resolveCollision(staticRects[i], playerRect, previousRect, collisionAxis, staticIndex, dynamicIndex) && collisionAxis == CollisionAxis::HORIZONTAL)
			staticIndex = i;
	}
	
	for(size_t i = 0; i < dynamicRects.size(); i++) {
		if (dynamicRects[i].isPlayer)
			continue;
		if (resolveCollision(dynamicRects[i], playerRect, previousRect, collisionAxis, staticIndex, dynamicIndex) && collisionAxis == CollisionAxis::HORIZONTAL)
			dynamicIndex = i;
	}
}

bool Player::resolveCollision(const Rect& rect, const Rect& playerRect, const Rect& previousRect, CollisionAxis collisionAxis, int staticIndex, int dynamicIndex) {
	if (SpriteEntity::HasCollision(rect.posX, rect.posY, rect.posX + rect.width, rect.posY + rect.height, playerRect.posX, playerRect.posY, playerRect.posX + playerRect.width, playerRect.posY + playerRect.height)) {
		rect.hasCollision = true;
		if (collisionAxis == CollisionAxis::HORIZONTAL) {			
			if (playerRect.posX <= rect.posX + rect.width && previousRect.posX >= rect.posX + rect.width){
				cell.posX = rect.posX + rect.width;
				return true;
			}

			if (playerRect.posX + playerRect.width >= rect.posX && previousRect.posX + previousRect.width <= rect.posX){
				cell.posX = rect.posX - 48.0f;
				return true;
			}
		}else {
			if (playerRect.posY <= rect.posY + rect.height && previousRect.posY >= rect.posY + rect.height)
				cell.posY = (staticIndex >= 0 && rect.posY + rect.height == staticRects[staticIndex].posY) || (dynamicIndex >= 0 && rect.posY + rect.height == dynamicRects[dynamicIndex].posY) ? cell.posY : rect.posY + rect.height + 56.0f /* + 0.1f*/;

			if (playerRect.posY + playerRect.height >= rect.posY && previousRect.posY + previousRect.height <= rect.posY)
				cell.posY = (staticIndex >= 0 && rect.posY - rect.height == staticRects[staticIndex].posY) || (dynamicIndex >= 0 && rect.posY - rect.height == dynamicRects[dynamicIndex].posY) ? cell.posY : rect.posY /* - 0.1f*/;
		}
	}
	return false;
}

bool Player::resolveCollision(const DynamicRect& rect, const Rect& playerRect, const Rect& previousRect, CollisionAxis collisionAxis, int staticIndex, int dynamicIndex) {
	if (SpriteEntity::HasCollision(rect.posX, rect.posY, rect.posX + rect.width, rect.posY + rect.height, playerRect.posX, playerRect.posY, playerRect.posX + playerRect.width, playerRect.posY + playerRect.height)) {
		rect.hasCollision = true;
		if (collisionAxis == CollisionAxis::HORIZONTAL) {
			if (playerRect.posX <= rect.posX + rect.width && previousRect.posX >= rect.previousRect.posX + rect.previousRect.width) {
				cell.posX = rect.posX + rect.width + 1.0f;
				return true;
			}

			if (playerRect.posX + playerRect.width >= rect.posX && previousRect.posX + previousRect.width <= rect.previousRect.posX) {
				cell.posX = rect.posX - 48.0f - 1.0f;
				return true;
			}
		}else {
			if (playerRect.posY <= rect.posY + rect.height && previousRect.posY >= rect.previousRect.posY + rect.previousRect.height)
				cell.posY = (staticIndex >= 0 && rect.posY + rect.height == staticRects[staticIndex].posY) || (dynamicIndex >= 0 && rect.posY + rect.height == dynamicRects[dynamicIndex].posY) ? cell.posY : rect.posY + rect.height + 56.0f /* + 0.1f*/;

			if (playerRect.posY + playerRect.height >= rect.posY && previousRect.posY + previousRect.height <= rect.previousRect.posY)
				cell.posY = (staticIndex >= 0 && rect.posY - rect.height == staticRects[staticIndex].posY) || (dynamicIndex >= 0 && rect.posY - rect.height == dynamicRects[dynamicIndex].posY) ? cell.posY : rect.posY /* - 0.1f*/;			
		}
	}
	return false;
}

void Player::checkContact() {

	Rect rightRect = { cell.posX + 48.0f, cell.posY - 42.0f , 2.0f, 28.0f };
	Rect leftRect = { cell.posX - 2.0f, cell.posY - 42.0f , 2.0f, 28.0f };
	Rect bottomRect = { cell.posX + 2.0f, cell.posY, 44.0f, 2.0f };
	Rect topRect = { cell.posX + 2.0f, cell.posY - 58.0f, 44.0f, 2.0f };
	
	int indexBottom, indexRight, indexLeft, indexTop;
	bool collideDynamic;
	
	m_collideRight = collideList(staticRects, dynamicRects, rightRect, indexRight, collideDynamic);
	m_collideDynamic = collideDynamic;
	m_collideLeft = collideList(staticRects, dynamicRects, leftRect, indexLeft, collideDynamic);
	m_collideDynamic |= collideDynamic;
	m_collideBottom = collideList(staticRects, dynamicRects, bottomRect, indexBottom, collideDynamic);
	m_collideTop = collideList(staticRects, dynamicRects, topRect, indexTop, collideDynamic);

	m_platformIndex = -1;
	for (size_t i = 0; i < dynamicRects.size(); i++) {
		if (dynamicRects[i].isMoving && SpriteEntity::HasCollision(dynamicRects[i].posX, dynamicRects[i].posY, dynamicRects[i].posX + dynamicRects[i].width, dynamicRects[i].posY + dynamicRects[i].height, bottomRect.posX, bottomRect.posY, bottomRect.posX + bottomRect.width, bottomRect.posY + bottomRect.height)) {
			m_platformIndex = i;
			if (m_collideBottom && indexBottom != m_platformIndex)
				m_platformIndex = -1;

			/*if (m_collideTop && indexTop != m_platformIndex) {
				float delta = collisionRect.posY - collisionRect.previousRect.posY;
				m_sizeY -= delta;
			}*/
		}
	}
}

void Player::platformMove() {
	
	if (m_platformIndex >= 0) {
		const DynamicRect& collisionRect = dynamicRects[m_platformIndex];
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

bool Player::collideList(const std::vector<Rect>& staticRects, const std::vector<DynamicRect>& dynamicRects, const Rect& rect, int& index, bool& collideDynamic) {
	index = -1;
	for (size_t i = 0; i < staticRects.size(); i++) {
		if (SpriteEntity::HasCollision(staticRects[i].posX, staticRects[i].posY, staticRects[i].posX + staticRects[i].width, staticRects[i].posY + staticRects[i].height, rect.posX, rect.posY, rect.posX + rect.width, rect.posY + rect.height)) {
			staticRects[i].hasCollision = true;
			index = i;
			collideDynamic = false;
			return true;
		}
	}

	for (size_t i = 0; i < dynamicRects.size(); i++) {
		if (dynamicRects[i].isPlayer)
			continue;

		if (SpriteEntity::HasCollision(dynamicRects[i].posX, dynamicRects[i].posY, dynamicRects[i].posX + dynamicRects[i].width, dynamicRects[i].posY + dynamicRects[i].height, rect.posX, rect.posY, rect.posX + rect.width, rect.posY + rect.height)) {
			dynamicRects[i].hasCollision = true;
			index = i;
			collideDynamic = true;
			return true;
		}
	}
	collideDynamic = false;
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
	m_wantReset = true;
}

void Player::setSizeX(float sizeX) {
	m_sizeX = sizeX;
}

void Player::setSizeY(float sizeY) {
	m_sizeY = sizeY;
}

float& Player::sizeX() {
	return m_sizeX;
}

float& Player::sizeY() {
	return m_sizeY;
}

const Rect& Player::getRect() {
	return collisionRect;
}