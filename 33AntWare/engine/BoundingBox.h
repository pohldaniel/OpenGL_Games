#pragma once

#include <vector>
#include <engine/Vector.h>

struct BoundingBox {

	BoundingBox();
	BoundingBox(BoundingBox const& rhs);
	BoundingBox(BoundingBox&& source);
	BoundingBox &operator=(const BoundingBox& rhs);


	BoundingBox(const Vector3f& min, const Vector3f& max);
	BoundingBox(float min, float max);

	~BoundingBox();

	BoundingBox transformed(const Matrix4f& transform) const;
	Vector3f getSize() const;

	void define(const Vector3f& point);	
	void inset(const Vector3f& min, const Vector3f& max) const;
	void setMin(const Vector3f& min) const;
	void setMax(const Vector3f& max) const;
	void minmize(float factor) const;
	void maximize(float factor) const;

	void createBuffer();
	void drawRaw() const;
	void cleanup();

	Vector3f min;
	Vector3f max;

private:

	void merge(const Vector3f& point);
	void merge(const Vector3f* vertices, size_t count);
	void undefine();

	Vector3f center() const;
	
	unsigned int m_vao = 0u;
	unsigned int m_vbo = 0u;
	unsigned int m_ibo = 0u;
};
