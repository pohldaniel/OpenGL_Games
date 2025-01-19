#pragma once

#include <engine/Vector.h>
#include <engine/BoundingBox.h>
#include <engine/Frustum.h>

class Sphere{
public:
	/// Sphere center.
	Vector3f center;
	/// Sphere radius.
	float radius;

	/// Construct as undefined (negative radius.)
	Sphere() :
		center(Vector3f::ZERO),
		radius(-FLT_MAX)
	{
	}

	/// Copy-construct.
	Sphere(const Sphere& sphere) :
		center(sphere.center),
		radius(sphere.radius)
	{
	}

	/// Construct from center and radius.
	Sphere(const Vector3f& center_, float radius_) :
		center(center_),
		radius(radius_)
	{
	}

	/// Construct from an array of vertices.
	Sphere(const Vector3f* vertices, size_t count)
	{
		Define(vertices, count);
	}

	/// Construct from a bounding box.
	Sphere(const BoundingBox& box)
	{
		Define(box);
	}

	/// Construct from a frustum.
	Sphere(const Frustum& frustum)
	{
		Define(frustum);
	}

	/// Assign from another sphere.
	Sphere& operator = (const Sphere& rhs)
	{
		center = rhs.center;
		radius = rhs.radius;
		return *this;
	}

	/// Test for equality with another sphere without epsilon.
	bool operator == (const Sphere& rhs) const { return center == rhs.center && radius == rhs.radius; }
	/// Test for inequality with another sphere without epsilon.
	bool operator != (const Sphere& rhs) const { return !(*this == rhs); }

	/// Define from another sphere.
	void Define(const Sphere& sphere)
	{
		center = sphere.center;
		radius = sphere.radius;
	}

	/// Define from center and radius.
	void Define(const Vector3f& center_, float radius_)
	{
		center = center_;
		radius = radius_;
	}

	/// Define from an array of vertices.
	void Define(const Vector3f* vertices, size_t count);
	/// Define from a bounding box.
	void Define(const BoundingBox& box);
	/// Define from a frustum.
	void Define(const Frustum& frustum);

	/// Merge a point.
	void Merge(const Vector3f& point)
	{
		// If undefined, set initial dimensions
		if (!IsDefined())
		{
			center = point;
			radius = 0.0f;
			return;
		}

		Vector3f offset = point - center;
		float dist = offset.length();

		if (dist > radius)
		{
			float half = (dist - radius) * 0.5f;
			radius += half;
			center += (half / dist) * offset;
		}
	}

	/// Set as undefined to allow the next merge to set initial size.
	void Undefine()
	{
		radius = -FLT_MAX;
	}

	/// Merge an array of vertices.
	void Merge(const Vector3f* vertices, size_t count);
	/// Merge a bounding box.
	void Merge(const BoundingBox& box);
	/// Merge a frustum.
	void Merge(const Frustum& frustum);
	/// Merge a sphere.
	void Merge(const Sphere& sphere);

	/// Return whether has non-negative radius.
	bool IsDefined() const { return radius >= 0.0f; }

	/// Return local-space point on the sphere.
	Vector3f LocalPoint(float theta, float phi) const;
	/// Return world-space point on the sphere.
	Vector3f Point(float theta, float phi) const { return center + LocalPoint(theta, phi); }

	/// Return distance of a point to the surface, or 0 if inside.
	float Distance(const Vector3f& point) const { return std::max((point - center).length() - radius, 0.0f); }
};
