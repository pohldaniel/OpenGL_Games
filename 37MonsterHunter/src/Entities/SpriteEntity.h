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

	const Cell& getCell();
	void setDirection(const Vector2f& direction);
	const ViewDirection& getViewDirection();

	static Vector2f GetDirection(std::string direction);

protected:

	void updateAnimation(float dt);
	int getFrameOffset(ViewDirection viewDirection);

	Cell& cell;
	int m_startFrame;
	float m_elapsedTime;
	int m_frameCount;
	ViewDirection m_viewDirection;
	ViewDirection m_lastViewDirection;
	Vector2f m_direction;
};