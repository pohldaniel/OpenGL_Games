#pragma once

#include <engine/Rect.h>
#include <engine/Camera.h>
#include "SpriteEntity.h"

enum CollisionAxis {
	HORIZONTAL,
	VERTICAL
};

class Player : public SpriteEntity {

public:

	Player(Cell& cell, Camera& camera, const std::vector<Rect>& collisionRects, float elpasedTime = 0.0f, int framecount = 4);
	virtual ~Player();

	void update(float dt) override;
	void setViewWidth(float viewWidth);
	void setViewHeight(float viewHeight);
	void setMapHeight(float mapHeight);
	void reset();

	const Rect getBottomRect();
	const Rect getLeftRect();
	const Rect getRightRect();

private:

	Rect getRect() override;
	void checkContact();
	bool collideList(const std::vector<Rect>& collisionRects, const Rect& rect);

	float m_mapHeight, m_viewWidth, m_viewHeight;
	float m_initialX, m_initialY;
	float m_gravity, m_jumpHeight;
	bool m_jump, m_collideBottom, m_collideLeft, m_collideRight;
	const std::vector<Rect>& collisionRects;
	Camera& camera;
	Vector2f m_direction, m_inputVector;
	void collision(const Rect& playerRect, const Rect& previousRect, CollisionAxis collisionAxis);
};