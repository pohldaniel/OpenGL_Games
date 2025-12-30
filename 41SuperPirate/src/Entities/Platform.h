#pragma once
#include <array>
#include <engine/Vector.h>
#include "Entity_new.h"
#include "DataStructs.h"

enum MoveDirection {
	P_HORIZONTAL,
	P_VERTICAL
};

class Platform : public EntityNew {

public:

	Platform(CollisionRect& dynmaicRect, MoveDirection moveDirection, float speed, const Vector2f& start = Vector2f::ZERO, const Vector2f& end = Vector2f::ZERO);
	virtual ~Platform();

	void update(float dt) override;
	const CollisionRect& getDynamicRect();
	Rect getRect();

private:

	void checkBoreder();

	CollisionRect& dynmaicRect;
	Vector2f m_start, m_end;
	MoveDirection m_moveDirection;
	float m_speed;

	float m_direction;
};