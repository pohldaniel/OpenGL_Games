#pragma once

#include <vector>
#include "Vector.h"

struct BoundingBox {

	enum Intersection {
		OUTSIDE = 0,
		INTERSECTS,
		INSIDE
	};

	BoundingBox();
	BoundingBox(BoundingBox const& rhs);
	BoundingBox(BoundingBox&& source);
	BoundingBox& operator=(const BoundingBox& rhs);
	BoundingBox& operator=(BoundingBox&& rhs);

	BoundingBox(const Vector3f& min, const Vector3f& max);
	BoundingBox(float min, float max);

	~BoundingBox();

	BoundingBox transformed(const Matrix4f& transform) const;
	Vector3f getSize() const;

	void define(const Vector3f& point);	
	void inset(const Vector3f& min, const Vector3f& max) const;
	void setMin(const Vector3f& min) const;
	void setMax(const Vector3f& max) const;
	void set(const Vector3f& min, const Vector3f& max) const;
	void minmize(float factor) const;
	void maximize(float factor) const;
	bool isColliding(Vector3f point) const;
	bool isColliding(const BoundingBox &box) const;
	Vector3f getCenter() const;
	Vector3f getHalfSize() const;
	void merge(const BoundingBox& box);
	void merge(const Vector3f& min, const Vector3f& max);
	float distance(const Vector3f & point) const;
	std::pair<float, float> projected(const Vector3f& axis) const;

	void createBuffer();
	void drawRaw() const;
	void cleanup();
	Intersection isInside(const BoundingBox& box) const;

	Vector3f min;
	Vector3f max;

private:

	void merge(const Vector3f& point);
	void merge(const Vector3f* vertices, size_t count);
	void undefine();

	
	
	unsigned int m_vao = 0u;
	unsigned int m_vbo = 0u;
	unsigned int m_ibo = 0u;
};
