#pragma once

#include <engine/Vector.h>
#include <engine/Transform.h>

class Object {

public:

	Object();
	Object(Object const& rhs);
	Object& operator=(const Object& rhs);
	Object(Object&& rhs);
	Object& operator=(Object&& rhs);

	virtual void setScale(const float sx, const float sy, const float sz);
	virtual void setScale(const Vector3f& scale);
	virtual void setScale(const float s);

	virtual void setPosition(const float x, const float y, const float z);
	virtual void setPosition(const Vector3f& position);

	virtual void setOrientation(const Vector3f& axis, float degrees);
	virtual void setOrientation(const float degreesX, const float degreesY, const float degreesZ);	
	virtual void setOrientation(const Vector3f& eulerAngle);
	virtual void setOrientation(const Quaternion& orientation);

	virtual void translate(const Vector3f& trans);
	virtual void translate(const float dx, const float dy, const float dz);

	virtual void translateRelative(const Vector3f& trans);
	virtual void translateRelative(const float dx, const float dy, const float dz);
	
	virtual void scale(const Vector3f& scale);
	virtual void scale(const float sx, const float sy, const float sz);
	virtual void scale(const float s);

	virtual void rotate(const float pitch, const float yaw, const float roll);
	virtual void rotate(const Vector3f& eulerAngle);
	virtual void rotate(const Vector3f& axis, float degrees);
	virtual void rotate(const Quaternion& orientation);

	virtual const Vector3f& getPosition() const;
	virtual const Vector3f& getScale() const;
	virtual const Quaternion& getOrientation() const;
	//const Vector3f& getRotation() const;

	const Matrix4f& getTransformationSOP() const;
	const Matrix4f& getTransformationSO() const;
	const Matrix4f& getTransformationSP() const;
	const Matrix4f& getTransformationOP() const;
	const Matrix4f& getTransformationO() const;
	const Matrix4f& getTransformationP() const;
	const Matrix4f& getTransformationS() const;

	static const Matrix4f &GetTransformation();

protected:

	Vector3f m_position;
	Vector3f m_scale;
	//Vector3f m_rotation;
	Quaternion m_orientation;

private:

	static Matrix4f Transformation;
};