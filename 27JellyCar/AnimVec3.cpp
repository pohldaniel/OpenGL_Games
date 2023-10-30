#include <cmath>
#include "AnimVec3.h"

AnimVec3 operator+(const AnimVec3& l, const AnimVec3& r) {
	return AnimVec3(l.x + r.x, l.y + r.y, l.z + r.z);
}

AnimVec3 operator-(const AnimVec3& l, const AnimVec3& r) {
	return AnimVec3(l.x - r.x, l.y - r.y, l.z - r.z);
}

AnimVec3 operator*(const AnimVec3& v, float f) {
	return AnimVec3(v.x * f, v.y * f, v.z * f);
}

AnimVec3 operator*(const AnimVec3& l, const AnimVec3& r) {
	return AnimVec3(l.x * r.x, l.y * r.y, l.z * r.z);
}

float dot(const AnimVec3& l, const AnimVec3& r) {
	return l.x * r.x + l.y * r.y + l.z * r.z;
}

float lenSq(const AnimVec3& v) {
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

float len(const AnimVec3& v) {
	float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
	if (lenSq < VEC3_EPSILON) {
		return 0.0f;
	}
	return sqrtf(lenSq);
}

void normalize(AnimVec3& v) {
	float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
	if (lenSq < VEC3_EPSILON) {
		return;
	}
	float invLen = 1.0f / sqrtf(lenSq);

	v.x *= invLen;
	v.y *= invLen;
	v.z *= invLen;
}

AnimVec3 normalized(const AnimVec3& v) {
	float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
	if (lenSq < VEC3_EPSILON) {
		return v;
	}
	float invLen = 1.0f / sqrtf(lenSq);

	return AnimVec3(
		v.x * invLen,
		v.y * invLen,
		v.z * invLen
	);
}

float angle(const AnimVec3& l, const AnimVec3& r) {
	float sqMagL = l.x * l.x + l.y * l.y + l.z * l.z;
	float sqMagR = r.x * r.x + r.y * r.y + r.z * r.z;

	if (sqMagL < VEC3_EPSILON || sqMagR < VEC3_EPSILON) {
		return 0.0f;
	}

	float dot = l.x * r.x + l.y * r.y + l.z * r.z;
	float len = sqrtf(sqMagL) * sqrtf(sqMagR);
	return acosf(dot / len);
}

AnimVec3 project(const AnimVec3& a, const AnimVec3& b) {
	float magBSq = len(b);
	if (magBSq < VEC3_EPSILON) {
		return AnimVec3();
	}
	float scale = dot(a, b) / magBSq;
	return b * scale;
}

AnimVec3 reject(const AnimVec3& a, const AnimVec3& b) {
	AnimVec3 projection = project(a, b);
	return a - projection;
}

AnimVec3 reflect(const AnimVec3& a, const AnimVec3& b) {
	float magBSq = len(b);
	if (magBSq < VEC3_EPSILON) {
		return AnimVec3();
	}
	float scale = dot(a, b) / magBSq;
	AnimVec3 proj2 = b * (scale * 2);
	return a - proj2;
}

AnimVec3 cross(const AnimVec3& l, const AnimVec3& r) {
	return AnimVec3(
		l.y * r.z - l.z * r.y,
		l.z * r.x - l.x * r.z,
		l.x * r.y - l.y * r.x
	);
}

AnimVec3 lerp(const AnimVec3& s, const AnimVec3& e, float t) {
	return AnimVec3(
		s.x + (e.x - s.x) * t,
		s.y + (e.y - s.y) * t,
		s.z + (e.z - s.z) * t
	);
}

AnimVec3 slerp(const AnimVec3& s, const AnimVec3& e, float t) {
	if (t < 0.01f) {
		return lerp(s, e, t);
	}

	AnimVec3 from = normalized(s);
	AnimVec3 to = normalized(e);

	float theta = angle(from, to);
	float sin_theta = sinf(theta);

	float a = sinf((1.0f - t) * theta) / sin_theta;
	float b = sinf(t * theta) / sin_theta;

	return from * a + to * b;
}

AnimVec3 nlerp(const AnimVec3& s, const AnimVec3& e, float t) {
	AnimVec3 linear(
		s.x + (e.x - s.x) * t,
		s.y + (e.y - s.y) * t,
		s.z + (e.z - s.z) * t
	);
	return normalized(linear);
}

bool operator==(const AnimVec3& l, const AnimVec3& r) {
	AnimVec3 diff(l - r);
	return lenSq(diff) < VEC3_EPSILON;
}

bool operator!=(const AnimVec3& l, const AnimVec3& r) {
	return !(l == r);
}