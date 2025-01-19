#include "Sphere.h"

void Sphere::Define(const Vector3f* vertices, size_t count)
{
	Undefine();
	Merge(vertices, count);
}

void Sphere::Define(const BoundingBox& box)
{
	const Vector3f& min = box.min;
	const Vector3f& max = box.max;

	Undefine();
	Merge(min);
	Merge(Vector3f(max[0], min[1], min[2]));
	Merge(Vector3f(min[0], max[1], min[2]));
	Merge(Vector3f(max[0], max[1], min[2]));
	Merge(Vector3f(min[0], min[1], max[2]));
	Merge(Vector3f(max[0], min[1], max[2]));
	Merge(Vector3f(min[0], max[1], max[2]));
	Merge(max);
}

void Sphere::Define(const Frustum& frustum)
{
	//Define(frustum.vertices, NUM_FRUSTUM_VERTICES);
}



void Sphere::Merge(const Vector3f* vertices, size_t count)
{
	while (count--)
		Merge(*vertices++);
}

void Sphere::Merge(const BoundingBox& box)
{
	const Vector3f& min = box.min;
	const Vector3f& max = box.max;

	Merge(min);
	Merge(Vector3f(max[0], min[1], min[2]));
	Merge(Vector3f(min[0], max[1], min[2]));
	Merge(Vector3f(max[0], max[1], min[2]));
	Merge(Vector3f(min[0], min[1], max[2]));
	Merge(Vector3f(max[0], min[1], max[2]));
	Merge(Vector3f(min[0], max[1], max[2]));
	Merge(max);
}

void Sphere::Merge(const Frustum& frustum)
{
	//const Vector3f* vertices = frustum.vertices;
	//Merge(vertices, NUM_FRUSTUM_VERTICES);
}

void Sphere::Merge(const Sphere& sphere)
{
	// If undefined, set initial dimensions
	if (!IsDefined())
	{
		center = sphere.center;
		radius = sphere.radius;
		return;
	}

	Vector3f offset = sphere.center - center;
	float dist = offset.length();

	// If sphere fits inside, do nothing
	if (dist + sphere.radius < radius)
		return;

	// If we fit inside the other sphere, become it
	if (dist + radius < sphere.radius)
	{
		center = sphere.center;
		radius = sphere.radius;
	}
	else
	{
		Vector3f normalizedOffset = offset / dist;

		Vector3f min = center - radius * normalizedOffset;
		Vector3f max = sphere.center + sphere.radius * normalizedOffset;
		center = (min + max) * 0.5f;
		radius = (max - center).length();
	}
}

Vector3f Sphere::LocalPoint(float theta, float phi) const{
	theta = theta * PI_ON_180;
	phi = phi * PI_ON_180;

	return Vector3f(
		radius * std::sinf(theta) * std::sinf(phi),
		radius * std::cosf(phi),
		radius * std::cosf(theta) * std::sinf(phi)
	);
}
