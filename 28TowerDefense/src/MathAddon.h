#pragma once
#include <engine/Vector.h>

class MathAddon{

public:

	static float angleRadToDeg(float angle);
	static float angleDegToRad(float angle);

	static bool checkOverlapLineCircle(const Vector2f& posLine1, const Vector2f& posLine2, const Vector2f& posCircle, float radiusCircle);

	static const float accGravity;
};