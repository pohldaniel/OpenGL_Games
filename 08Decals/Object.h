#pragma once
#include "engine/Vector.h"

class Object {

public:
	Object();

	void setOrigin(const float x, const float y, const float z);
	void setOrigin(const Vector3f &origin);
	virtual void setSize(const float x, const float y, const float z);
	virtual void setSize(const Vector3f &size);
	virtual void setPosition(const float x, const float y, const float z);
	virtual void setPosition(const Vector3f &position);

	const Vector3f &getPosition() const;
	const Vector3f &getSize() const;
	const Vector3f &getOrigin() const;
	

protected:
	
	Vector3f m_position;
	Vector3f m_size;
	Vector3f m_origin;
	Matrix4f m_transform = Matrix4f::IDENTITY;
};