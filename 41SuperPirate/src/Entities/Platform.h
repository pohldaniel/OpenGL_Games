#pragma once
#include <array>
#include <engine/Vector.h>
#include <engine/Rect.h>
#include "Entity_new.h"


enum MoveDirection {
	P_HORIZONTAL,
	P_VERTICAL
};

class Platform : public EntityNew {

public:

	Platform(Rect& collisonRect, MoveDirection moveDirection, float speed, const Vector2f& start = Vector2f::ZERO, const Vector2f& end = Vector2f::ZERO);
	virtual ~Platform();

	void update(float dt) override;
	const Rect& getRect();
	const Rect& getPreviousRect();

private:

	void checkBoreder();

	Rect& rect;
	Vector2f m_start, m_end;
	MoveDirection m_moveDirection;
	float m_speed;
	Rect m_previousRect;

	float m_direction;
};