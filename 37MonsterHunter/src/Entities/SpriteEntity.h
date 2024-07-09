#pragma once
#include <Zone.h>

enum ViewDirection {
	LEFT,
	RIGHT,
	DOWN,
	UP,
	NONE
};

class SpriteEntity {

public:

	SpriteEntity(Cell& cell, float elpasedTime = 0.0f, int framecount = 4);
	virtual ~SpriteEntity();
	virtual void update(float dt) = 0;
	virtual const ViewDirection& getViewDirection();

	void setMovingSpeed(float movingSpeed);
	const Cell& getCell() const;
	void setViewDirection(ViewDirection direction);
	const Vector2f& getDirection() const;
	void changeFacingDirection(const SpriteEntity& target);

	static ViewDirection GetDirection(std::string direction);
	static Vector2f GetDirection(ViewDirection direction);
	static bool CheckConnection(const Cell& origin, const Cell& target, ViewDirection viewDirection, float radius = 100.0f, float tolerance = 30.0f);
	static bool HasCollision(float r1_l, float r1_t, float r1_r, float r1_b, float r2_l, float r2_t, float r2_r, float r2_b);

protected:

	void updateAnimation(float dt);
	void updateLastViewDirection();
	void resetAnimation();
	int getFrameOffset(ViewDirection viewDirection);

	Cell& cell;
	int m_startFrame;
	int m_frameCount;
	float m_elapsedTime;
	float m_movingSpeed;
	ViewDirection m_viewDirection;
	ViewDirection m_lastViewDirection;
	Vector2f m_direction;
};