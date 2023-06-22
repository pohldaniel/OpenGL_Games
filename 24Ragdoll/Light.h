#pragma once

#include "engine/Vector.h"

struct Light{

	Vector3f position;
	Vector3f direction;
	Vector3f color;

	inline void update(const Vector3f& cameraPosition) {
		direction = Vector3f::Normalize(position - cameraPosition);
	}
};