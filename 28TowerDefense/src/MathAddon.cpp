#include "MathAddon.h"

float MathAddon::angleRadToDeg(float angle) {
	return angle * 180.0f / PI;
}

float MathAddon::angleDegToRad(float angle) {
	return angle * PI / 180.0f;
}