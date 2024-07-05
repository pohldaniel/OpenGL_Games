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
	void setViewWidth(float viewWidth);
	void setViewHeight(float viewHeight);
	void setMapHeight(float mapHeight);
	void block();
	void unblock();
	void adjustCamera();
	const ViewDirection& getViewDirection() override;
	const ViewDirection& getLastViewDirection();
	const std::vector<Rect>& getCollisionRects() const;
	bool isBlocked();

private:
	
	
	
	float m_mapHeight, m_viewWidth, m_viewHeight;
	const std::vector<Rect>& collisionRects;
	Camera& camera;
	bool m_blocked;
	
};