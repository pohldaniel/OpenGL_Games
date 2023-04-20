#pragma once

#include "engine/Vector.h"

class Object {

public:
	Object();

	virtual void setScale(const float x, const float y, const float z);
	virtual void setScale(const Vector3f &scale);
	virtual void setPosition(const float x, const float y, const float z);
	virtual void setPosition(const Vector3f &position);
	virtual void setOrientation(const Vector3f &axis, float degrees);
	virtual void setOrientation(const float degreesX, const float degreesY, const float degreesZ);

	const Vector3f &getPosition();
	const Vector3f &getScale();
	const Quaternion &getOrientation();

protected:

	Vector3f m_position;
	Vector3f m_scale;
	Quaternion m_orientation;
};