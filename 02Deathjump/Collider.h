#pragma once
#include "Vector.h"

struct Collider{
	Collider(const Vector2f& s = Vector2f(), const Vector2f& p = Vector2f()): size(s), position(p) { }

	Vector2f size;
	Vector2f position;
};