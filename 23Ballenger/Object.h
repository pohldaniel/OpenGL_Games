#pragma once

#include "engine/Vector.h"
#include "engine/Transform.h"

class Object {

public:

	Object();

	virtual void setScale(const float sx, const float sy, const float sz);
	virtual void setScale(const Vector3f &scale);
	virtual void setScale(const float s);
	virtual void setPosition(const float x, const float y, const float z);
	virtual void setPosition(const Vector3f &position);
	virtual void setOrientation(const Vector3f &axis, float degrees);
	virtual void setOrientation(const float degreesX, const float degreesY, const float degreesZ);	
	virtual void setOrientation(const Vector3f &eulerAngle);
	virtual void setOrientation(const Quaternion &orientation);

	virtual void translate(const Vector3f &trans);
	virtual void translate(const float dx, const float dy, const float dz);
	virtual void scale(const Vector3f &scale);
	virtual void scale(const float sx, const float sy, const float sz);
	virtual void scale(const float s);

	virtual void rotate(const float pitch, const float yaw, const float roll);
	virtual void rotate(const Vector3f &axis, float degrees);

	const Vector3f &getPosition() const;
	const Vector3f &getScale() const;
	const Quaternion &getOrientation() const;

	const Matrix4f &getTransformationSOP();
	const Matrix4f &getTransformationSO();
	const Matrix4f &getTransformationSP();
	const Matrix4f &getTransformationOP();
	const Matrix4f &getTransformationO();
	const Matrix4f &getTransformationP();
	const Matrix4f &getTransformationS();

	static const Matrix4f &GetTransformation();

protected:

	Vector3f m_position;
	Vector3f m_scale;
	Quaternion m_orientation;

private:

	static Matrix4f Transformation;
};