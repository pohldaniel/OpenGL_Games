#pragma once

#include "Vector.h"

class Object2D {

public:

	Object2D();
	Object2D(Object2D const& rhs);
	Object2D& operator=(const Object2D& rhs);
	Object2D(Object2D&& rhs) noexcept;
	Object2D& operator=(Object2D&& rhs) noexcept;
	virtual ~Object2D();

	virtual void setScale(float sx, float sy);
	virtual void setScale(const Vector2f& scale);
	virtual void setScale(float s);

	virtual void setPosition(float x, float y);
	virtual void setPosition(const Vector2f& position);

	virtual void setOrigin(float x, float y);
	virtual void setOrigin(const Vector2f& origin);

	virtual void setOrientation(float degrees);

	virtual void translate(const Vector2f& trans);
	virtual void translate(float dx, float dy);

	virtual void translateRelative(const Vector2f& trans);
	virtual void translateRelative(float dx, float dy);

	virtual void scale(const Vector2f& scale);
	virtual void scale(float sx, float sy);
	virtual void scale(float s);

	virtual void rotate(float degrees);

	const Vector2f& getPosition() const;
	const Vector2f& getScale() const;
	float getOrientation() const;

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

	static thread_local Matrix4f Transformation;
};

class Object {

public:

	Object();
	Object(Object const& rhs);
	Object& operator=(const Object& rhs);
	Object(Object&& rhs) noexcept;
	Object& operator=(Object&& rhs) noexcept;

	virtual void setScale(float sx, float sy, float sz) const;
	virtual void setScale(const Vector3f& scale) const;
	virtual void setScale(float s) const;

	virtual void setPosition(float x, float y, float z) const;
	virtual void setPosition(const Vector3f& position) const;

	virtual void setOrientation(const Vector3f& axis, float degrees) const;
	virtual void setOrientation(float degreesX, float degreesY, float degreesZ) const;
	virtual void setOrientation(const Vector3f& eulerAngle) const;
	virtual void setOrientation(const Quaternion& orientation) const;
	virtual void setOrientation(float x, float y, float z, float w) const;

	virtual void translate(const Vector3f& trans);
	virtual void translate(float dx, float dy, float dz);

	virtual void translateRelative(const Vector3f& trans);
	virtual void translateRelative(float dx, float dy, float dz);
	
	virtual void scale(const Vector3f& scale);
	virtual void scale(float sx, float sy, float sz);
	virtual void scale(float s);

	virtual void rotate(float pitch, float yaw, float roll);
	virtual void rotate(const Vector3f& eulerAngle);
	virtual void rotate(const Vector3f& axis, float degrees);
	virtual void rotate(const Quaternion& orientation);
	virtual void rotate(float x, float y, float z, float w);

	const Vector3f& getPosition() const;
	const Vector3f& getScale() const;
	const Quaternion& getOrientation() const;
	Vector3f& getPosition();
	Vector3f& getScale();
	Quaternion& getOrientation();

	const Matrix4f& getTransformationSOP() const;
	const Matrix4f& getTransformationSO() const;
	const Matrix4f& getTransformationSP() const;
	const Matrix4f& getTransformationOP() const;
	const Matrix4f& getTransformationO() const;
	const Matrix4f& getTransformationP() const;
	const Matrix4f& getTransformationS() const;

	static const Matrix4f &GetTransformation();

protected:

	mutable Vector3f m_position;
	mutable Vector3f m_scale;
	mutable Quaternion m_orientation;

	static thread_local Matrix4f Transformation;
};