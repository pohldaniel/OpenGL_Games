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
	BoundingBox(const Vector3f& center, float radius);

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
	const BoundingBox minmize(float factor);
	const BoundingBox maximize(float factor);


	bool isColliding(Vector3f point) const;
	bool isColliding(const BoundingBox &box) const;
	Vector3f getCenter() const;
	Vector3f getHalfSize() const;
	void merge(const BoundingBox& box);
	void merge(const Vector3f& min, const Vector3f& max);
	float distance(const Vector3f & point) const;
	std::pair<float, float> projected(const Vector3f& axis) const;
	void reset();
	
	void createBuffer();
	void drawRaw() const;
	void cleanup();
	Intersection isInside(const BoundingBox& box) const;
	Intersection isInsideFast(const BoundingBox& box) const;
	Intersection isInside(const Vector3f& point) const;

	Vector3f min;
	Vector3f max;

private:

	void merge(const Vector3f& point);
	void merge(const Vector3f* vertices, size_t count);
	void undefine();

	
	bool m_markForDelete;
	unsigned int m_vao = 0u;
	unsigned int m_vbo = 0u;
	unsigned int m_ibo = 0u;
};
