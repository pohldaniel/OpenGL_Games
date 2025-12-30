#pragma once

#include <engine/Camera.h>
#include <engine/Timer.h>

#include "DataStructs.h"
#include "SpriteEntity.h"

enum CollisionAxis {
	HORIZONTAL,
	VERTICAL
};

class Player : public SpriteEntity {

public:

	Player(Cell& cell, Camera& camera, const std::vector<CollisionRect>& collisionRects, float elpasedTime = 0.0f, int framecount = 4);
	virtual ~Player();

	void update(float dt) override;
	void setViewWidth(float viewWidth);
	void setViewHeight(float viewHeight);
	void setMapHeight(float mapHeight);
	void setMovingSpeed(float movingSpeed);
	void reset();

	const Rect getBottomRect();
	const Rect getLeftRect();
	const Rect getRightRect();

private:

	Rect getRect();
	void checkContact();
	bool collideList(const std::vector<CollisionRect>& collisionRects, const Rect& rect);
	void collision(const Rect& playerRect, const Rect& previousRect, CollisionAxis collisionAxis);
	bool compare(const Rect& rect1, const Rect& rect2);


	float m_mapHeight, m_viewWidth, m_viewHeight;
	float m_initialX, m_initialY;
	float m_gravity, m_jumpHeight;
	bool m_jump, m_collideBottom, m_collideLeft, m_collideRight;
	bool m_onWall, m_wasCollideLeft, m_wasCollideRight, m_wantJump, m_waitForCollideBottom;
	bool m_wallBounceLeft, m_wallBounceRight;
	Timer m_wallJumpTimer;
	const std::vector<CollisionRect>& collisionRects;
	Camera& camera;
	Vector2f m_direction, m_inputVector;
	float m_movingSpeed;
	Rect m_previousRect;
};