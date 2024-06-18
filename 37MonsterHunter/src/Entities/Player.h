#pragma once
#include <vector>
#include <engine/Vector.h>
#include <engine/Rect.h>
#include <engine/Camera.h>
#include <Entities/SpriteEntity.h>

class Player : public SpriteEntity {

public:

	Player(Cell& cell, Camera& camera, const std::vector<Rect>& collisionRects);
	virtual ~Player();

	void update(float dt) override;
	void setMovingSpeed(float movingSpeed);	
	void setViewWidth(float viewWidth);
	void setViewHeight(float viewHeight);
	void setMapHeight(float mapHeight);

private:

	bool hasCollision(float r1_l, float r1_t, float r1_r, float r1_b, float r2_l, float r2_t, float r2_r, float r2_b);
	

	float m_movingSpeed, m_mapHeight, m_viewWidth, m_viewHeight;
	const std::vector<Rect>& collisionRects;
	Camera& camera;
};