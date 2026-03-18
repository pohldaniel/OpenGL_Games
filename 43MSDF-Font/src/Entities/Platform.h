#pragma once
#include <array>
#include <engine/Vector.h>

#include "Entity2D.h"
#include "DataStructs.h"

enum MoveDirection {
	P_HORIZONTAL,
	P_VERTICAL
};

class Platform : public Entity2D {

public:

	Platform(DynamicRect& dynmaicRect, MoveDirection moveDirection, float speed, const Vector2f& start = Vector2f::ZERO, const Vector2f& end = Vector2f::ZERO);
	virtual ~Platform();

	void update(float dt) override;
	const DynamicRect& getDynamicRect();
	Rect getRect();

private:

	void checkBoreder();

	DynamicRect& dynmaicRect;
	Vector2f m_start, m_end;
	MoveDirection m_moveDirection;
	float m_speed;

	float m_direction;
};