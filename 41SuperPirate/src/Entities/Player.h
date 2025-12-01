#pragma once

#include <engine/Rect.h>
#include <engine/Camera.h>
#include "SpriteEntity.h"

class Player : public SpriteEntity {

public:

	Player(Cell& cell, Camera& camera, const std::vector<Rect>& collisionRects, float elpasedTime = 0.0f, int framecount = 4);
	virtual ~Player();

	void update(float dt) override;
	void setViewWidth(float viewWidth);
	void setViewHeight(float viewHeight);
	void setMapHeight(float mapHeight);
	void reset();

private:

	float m_mapHeight, m_viewWidth, m_viewHeight;
	float m_initialX, m_initialY;
	const std::vector<Rect>& collisionRects;
	Camera& camera;
};