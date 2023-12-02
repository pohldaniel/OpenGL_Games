#include "MathAddon.h"

const float MathAddon::accGravity = -(9.81f * 10.0f);

float MathAddon::angleRadToDeg(float angle) {
	return angle * 180.0f / PI;
}

float MathAddon::angleDegToRad(float angle) {
	return angle * PI / 180.0f;
}

bool MathAddon::checkOverlapLineCircle(const Vector2f& posLine1, const Vector2f& posLine2, const Vector2f& posCircle, float radiusCircle) {
	//Check to see if the line and cirlce overlap or not.
	Vector2f lengthCurrent = posLine1 - posLine2;
	Vector2f normalParallel = Vector2f(lengthCurrent).normalize();
	float distanceParallel = normalParallel.dot(posCircle - posLine2);

	float distanceCheck = 0.0f;
	if (distanceParallel < 0.0f)
		//The circle's center is past posLine2 so the check will see if it's within the circle.
		distanceCheck = (posLine2 - posCircle).length();
	else if (distanceParallel > lengthCurrent.length())
		//The circle's center is past posLine1 so the check will see if it's within the circle.
		distanceCheck = (posLine1 - posCircle).length();
	else {
		//The perpendicular distance from the line to the center of the circle will be checked.
		Vector2f normalPerpendicular = normalParallel.getNegativeReciprocal();
		distanceCheck = abs(normalPerpendicular.dot(posLine2 - posCircle));
	}

	return (distanceCheck < radiusCircle);
}