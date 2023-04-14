#pragma once

#include "engine/Transform.h"

class Object {

public:
	Object();

	virtual void scale(const float x, const float y, const float z);
	virtual void scale(const Vector3f &scale);
	virtual void setPosition(const float x, const float y, const float z);
	virtual void setPosition(const Vector3f &position);

	const Vector3f &getPosition();
	const Vector3f &getScale();

protected:
	Vector3f m_position;
	Vector3f m_scale;
	//Transform m_transform;
};