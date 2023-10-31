#pragma once

#define VEC3_EPSILON 0.000001f

struct AnimVec3 {
	union {
		struct {
			float x;
			float y;
			float z;
		};
		float v[3];
	};
	inline AnimVec3() : x(0.0f), y(0.0f), z(0.0f) { }
	inline AnimVec3(float _x, float _y, float _z) :
		x(_x), y(_y), z(_z) { }
	inline AnimVec3(float* fv) :
		x(fv[0]), y(fv[1]), z(fv[2]) { }
};

AnimVec3 operator+(const AnimVec3& l, const AnimVec3& r);
AnimVec3 operator-(const AnimVec3& l, const AnimVec3& r);
AnimVec3 operator*(const AnimVec3& v, float f);
AnimVec3 operator*(const AnimVec3& l, const AnimVec3& r);
float dot(const AnimVec3& l, const AnimVec3& r);
float lenSq(const AnimVec3& v);
float len(const AnimVec3& v);
void normalize(AnimVec3& v);
AnimVec3 normalized(const AnimVec3& v);
float angle(const AnimVec3& l, const AnimVec3& r);
AnimVec3 project(const AnimVec3& a, const AnimVec3& b);
AnimVec3 reject(const AnimVec3& a, const AnimVec3& b);
AnimVec3 reflect(const AnimVec3& a, const AnimVec3& b);
AnimVec3 cross(const AnimVec3& l, const AnimVec3& r);
AnimVec3 lerp(const AnimVec3& s, const AnimVec3& e, float t);
AnimVec3 slerp(const AnimVec3& s, const AnimVec3& e, float t);
AnimVec3 nlerp(const AnimVec3& s, const AnimVec3& e, float t);
bool operator==(const AnimVec3& l, const AnimVec3& r);
bool operator!=(const AnimVec3& l, const AnimVec3& r);