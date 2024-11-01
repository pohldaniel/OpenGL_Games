#pragma once

#include "Vector.h"

class Object2D {

public:

	Object2D();
	Object2D(Object2D const& rhs);
	Object2D& operator=(const Object2D& rhs);
	Object2D(Object2D&& rhs);
	Object2D& operator=(Object2D&& rhs);
	virtual ~Object2D();

	virtual void setScale(const float sx, const float sy);
	virtual void setScale(const Vector2f& scale);
	virtual void setScale(const float s);

	virtual void setPosition(const float x, const float y);
	virtual void setPosition(const Vector2f& position);

	virtual void setOrigin(const float x, const float y);
	virtual void setOrigin(const Vector2f& origin);

	virtual void setOrientation(const float degrees);

	virtual void translate(const Vector2f& trans);
	virtual void translate(const float dx, const float dy);

	virtual void translateRelative(const Vector2f& trans);
	virtual void translateRelative(const float dx, const float dy);

	virtual void scale(const Vector2f& scale);
	virtual void scale(const float sx, const float sy);
	virtual void scale(const float s);

	virtual void rotate(const float degrees);

	const Vector2f& getPosition() const;
	const Vector2f& getScale() const;
	const float getOrientation() const;

	const Matrix4f& getTransformationSOP() const;
	const Matrix4f& getTransformationSO() const;
	const Matrix4f& getTransformationSP() const;
	const Matrix4f& getTransformationOP() const;
	const Matrix4f& getTransformationO() const;
	const Matrix4f& getTransformationP() const;
	const Matrix4f& getTransformationS() const;

	static const Matrix4f &GetTransformation();

protected:

	Vector2f m_position;
	Vector2f m_origin;
	Vector2f m_scale;
	float m_orientation;

	static Matrix4f Transformation;
};

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

	virtual void setOrigin(const float x, const float y, const float z);
	virtual void setOrigin(const Vector3f& origin);

	virtual void setOrientation(const Vector3f& axis, float degrees);
	virtual void setOrientation(const float degreesX, const float degreesY, const float degreesZ);	
	virtual void setOrientation(const Vector3f& eulerAngle);
	virtual void setOrientation(const Quaternion& orientation);
	virtual void setOrientation(const float x, const float y, const float z, const float w);

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
	virtual void rotate(const float x, const float y, const float z, const float w);

	const Vector3f& getPosition() const;
	const Vector3f& getScale() const;
	const Quaternion& getOrientation() const;

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
	Vector3f m_origin;
	Vector3f m_scale;	
	Quaternion m_orientation;

	static Matrix4f Transformation;
};