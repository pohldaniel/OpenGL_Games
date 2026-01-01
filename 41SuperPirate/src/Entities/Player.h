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

	Player(Cell& cell, DynamicRect& collisionRect, Camera& camera, const std::vector<Rect>& staticRects, const std::vector<DynamicRect>& dynamicRects, float elpasedTime = 0.0f, int framecount = 4);
	virtual ~Player();

	void update(float dt) override;
	void setViewWidth(float viewWidth);
	void setViewHeight(float viewHeight);
	void setMapHeight(float mapHeight);
	void setMovingSpeed(float movingSpeed);
	void setSizeX(float sizeX);
	void setSizeY(float sizeY);
	void reset();

	float& sizeX();
	float& sizeY();

	const Rect getBottomRect();
	const Rect getLeftRect();
	const Rect getRightRect();
	const Rect getTopRect();

private:

	const Rect& getRect();
	void checkContact();
	void platformMove();
	bool collideList(const std::vector<Rect>& staticRects, const std::vector<DynamicRect>& dynamicRects, const Rect& rect, int& index, bool& collideDynamic);
	void collision(const Rect& playerRect, const Rect& previousRect, CollisionAxis collisionAxis, int& staticIndex, int& dynamicIndex);
	bool resolveCollision(const Rect& rect, const Rect& playerRect, const Rect& previousRect, CollisionAxis collisionAxis, int staticIndex, int dynamicIndex);
	bool resolveCollision(const DynamicRect& rect, const Rect& playerRect, const Rect& previousRect, CollisionAxis collisionAxis, int staticIndex, int dynamicIndex);

	float m_mapHeight, m_viewWidth, m_viewHeight;
	float m_initialX, m_initialY;
	float m_gravity, m_jumpHeight;
	float m_sizeX, m_sizeY;
	bool m_jumpPressed, m_wantJump, m_blockJump;
	bool m_collideBottom, m_collideLeft, m_collideRight, m_collideTop, m_waitForCollideBottom, m_wasCollideBottom, m_wasCollideLeft, m_wasCollideRight;
	bool m_wallBounceLeft, m_wallBounceRight;
	bool m_wantReset;
	Timer m_wallJumpTimer, m_allowJumpTimer;
	const std::vector<Rect>& staticRects;
	const std::vector<DynamicRect>& dynamicRects;
	Camera& camera;
	Vector2f m_direction, m_inputVector;
	float m_movingSpeed;
	int m_platformIndex;
	DynamicRect& collisionRect;
	int m_lastDynamic;
	bool m_collideDynamic, m_wasCollideDynamic, m_blockWallSlide, m_onWall;

};