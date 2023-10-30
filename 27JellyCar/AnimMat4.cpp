#include <cmath>
#include "AnimMat4.h"
#include <iostream>


bool operator==(const AnimMat4& a, const AnimMat4& b) {
	for (int i = 0; i < 16; ++i) {
		if (fabsf(a.v[i] - b.v[i]) > MAT4_EPSILON) {
			return false;
		}
	}
	return true;
}

bool operator!=(const AnimMat4& a, const AnimMat4& b) {
	return !(a == b);
}

AnimMat4 operator+(const AnimMat4& a, const AnimMat4& b) {
	return AnimMat4(
		a.xx + b.xx, a.xy + b.xy, a.xz + b.xz, a.xw + b.xw,
		a.yx + b.yx, a.yy + b.yy, a.yz + b.yz, a.yw + b.yw,
		a.zx + b.zx, a.zy + b.zy, a.zz + b.zz, a.zw + b.zw,
		a.tx + b.tx, a.ty + b.ty, a.tz + b.tz, a.tw + b.tw
	);
}

AnimMat4 operator*(const AnimMat4& m, float f) {
	return AnimMat4(
		m.xx * f, m.xy * f, m.xz * f, m.xw * f,
		m.yx * f, m.yy * f, m.yz * f, m.yw * f,
		m.zx * f, m.zy * f, m.zz * f, m.zw * f,
		m.tx * f, m.ty * f, m.tz * f, m.tw * f
	);
}

#define M4D(aRow, bCol) \
    a.v[0 * 4 + aRow] * b.v[bCol * 4 + 0] + \
    a.v[1 * 4 + aRow] * b.v[bCol * 4 + 1] + \
    a.v[2 * 4 + aRow] * b.v[bCol * 4 + 2] + \
    a.v[3 * 4 + aRow] * b.v[bCol * 4 + 3]

AnimMat4 operator*(const AnimMat4& a, const AnimMat4& b) {
	return AnimMat4(
		M4D(0, 0), M4D(1, 0), M4D(2, 0), M4D(3, 0), // Column 0
		M4D(0, 1), M4D(1, 1), M4D(2, 1), M4D(3, 1), // Column 1
		M4D(0, 2), M4D(1, 2), M4D(2, 2), M4D(3, 2), // Column 2
		M4D(0, 3), M4D(1, 3), M4D(2, 3), M4D(3, 3)  // Column 3
	);
}

#define M4V4D(mRow, x, y, z, w) \
    x * m.v[0 * 4 + mRow] + \
    y * m.v[1 * 4 + mRow] + \
    z * m.v[2 * 4 + mRow] + \
    w * m.v[3 * 4 + mRow]

AnimVec4 operator*(const AnimMat4& m, const AnimVec4& v) {
	return AnimVec4(
		M4V4D(0, v.x, v.y, v.z, v.w),
		M4V4D(1, v.x, v.y, v.z, v.w),
		M4V4D(2, v.x, v.y, v.z, v.w),
		M4V4D(3, v.x, v.y, v.z, v.w)
	);
}

AnimVec3 transformVector(const AnimMat4& m, const AnimVec3& v) {
	return AnimVec3(
		M4V4D(0, v.x, v.y, v.z, 0.0f),
		M4V4D(1, v.x, v.y, v.z, 0.0f),
		M4V4D(2, v.x, v.y, v.z, 0.0f)
	);
}

glm::vec3 transformVector(const AnimMat4& m, const glm::vec3& v) {
	return glm::vec3(
		M4V4D(0, v.x, v.y, v.z, 0.0f),
		M4V4D(1, v.x, v.y, v.z, 0.0f),
		M4V4D(2, v.x, v.y, v.z, 0.0f)
	);
}

AnimVec3 transformPoint(const AnimMat4& m, const AnimVec3& v) {
	return AnimVec3(
		M4V4D(0, v.x, v.y, v.z, 1.0f),
		M4V4D(1, v.x, v.y, v.z, 1.0f),
		M4V4D(2, v.x, v.y, v.z, 1.0f)
	);
}

glm::vec3 transformPoint(const AnimMat4& m, const glm::vec3& v) {
	return glm::vec3(
		M4V4D(0, v.x, v.y, v.z, 1.0f),
		M4V4D(1, v.x, v.y, v.z, 1.0f),
		M4V4D(2, v.x, v.y, v.z, 1.0f)
	);
}

AnimVec3 transformPoint(const AnimMat4& m, const AnimVec3& v, float& w) {
	float _w = w;
	w = M4V4D(3, v.x, v.y, v.z, _w);

	return AnimVec3(
		M4V4D(0, v.x, v.y, v.z, _w),
		M4V4D(1, v.x, v.y, v.z, _w),
		M4V4D(2, v.x, v.y, v.z, _w)
	);
}

#define M4SWAP(x, y) \
    {float t = x; x = y; y = t; }

void transpose(AnimMat4& m) {
	M4SWAP(m.yx, m.xy);
	M4SWAP(m.zx, m.xz);
	M4SWAP(m.tx, m.xw);
	M4SWAP(m.zy, m.yz);
	M4SWAP(m.ty, m.yw);
	M4SWAP(m.tz, m.zw);
}

AnimMat4 transposed(const AnimMat4& m) {
	return AnimMat4(
		m.xx, m.yx, m.zx, m.tx,
		m.xy, m.yy, m.zy, m.ty,
		m.xz, m.yz, m.zz, m.tz,
		m.xw, m.yw, m.zw, m.tw
	);
}

#define M4_3X3MINOR(c0, c1, c2, r0, r1, r2) \
    (m.v[c0 * 4 + r0] * (m.v[c1 * 4 + r1] * m.v[c2 * 4 + r2] - m.v[c1 * 4 + r2] * m.v[c2 * 4 + r1]) - \
     m.v[c1 * 4 + r0] * (m.v[c0 * 4 + r1] * m.v[c2 * 4 + r2] - m.v[c0 * 4 + r2] * m.v[c2 * 4 + r1]) + \
     m.v[c2 * 4 + r0] * (m.v[c0 * 4 + r1] * m.v[c1 * 4 + r2] - m.v[c0 * 4 + r2] * m.v[c1 * 4 + r1]))

float determinant(const AnimMat4& m) {
	return  m.v[0] * M4_3X3MINOR(1, 2, 3, 1, 2, 3)
		- m.v[4] * M4_3X3MINOR(0, 2, 3, 1, 2, 3)
		+ m.v[8] * M4_3X3MINOR(0, 1, 3, 1, 2, 3)
		- m.v[12] * M4_3X3MINOR(0, 1, 2, 1, 2, 3);
}

AnimMat4 adjugate(const AnimMat4& m) {
	// Cofactor(M[i, j]) = Minor(M[i, j]] * pow(-1, i + j)
	AnimMat4 cofactor;

	cofactor.v[0] = M4_3X3MINOR(1, 2, 3, 1, 2, 3);
	cofactor.v[1] = -M4_3X3MINOR(1, 2, 3, 0, 2, 3);
	cofactor.v[2] = M4_3X3MINOR(1, 2, 3, 0, 1, 3);
	cofactor.v[3] = -M4_3X3MINOR(1, 2, 3, 0, 1, 2);

	cofactor.v[4] = -M4_3X3MINOR(0, 2, 3, 1, 2, 3);
	cofactor.v[5] = M4_3X3MINOR(0, 2, 3, 0, 2, 3);
	cofactor.v[6] = -M4_3X3MINOR(0, 2, 3, 0, 1, 3);
	cofactor.v[7] = M4_3X3MINOR(0, 2, 3, 0, 1, 2);

	cofactor.v[8] = M4_3X3MINOR(0, 1, 3, 1, 2, 3);
	cofactor.v[9] = -M4_3X3MINOR(0, 1, 3, 0, 2, 3);
	cofactor.v[10] = M4_3X3MINOR(0, 1, 3, 0, 1, 3);
	cofactor.v[11] = -M4_3X3MINOR(0, 1, 3, 0, 1, 2);

	cofactor.v[12] = -M4_3X3MINOR(0, 1, 2, 1, 2, 3);
	cofactor.v[13] = M4_3X3MINOR(0, 1, 2, 0, 2, 3);
	cofactor.v[14] = -M4_3X3MINOR(0, 1, 2, 0, 1, 3);
	cofactor.v[15] = M4_3X3MINOR(0, 1, 2, 0, 1, 2);

	return transposed(cofactor);
}

AnimMat4 inverse(const AnimMat4& m) {
	float det = determinant(m);

	if (det == 0.0f) { // Epsilon check would need to be REALLY small
		std::cout << "WARNING: Trying to invert a matrix with a zero determinant\n";
		return AnimMat4();
	}
	AnimMat4 adj = adjugate(m);

	return adj * (1.0f / det);
}

void invert(AnimMat4& m) {
	float det = determinant(m);

	if (det == 0.0f) {
		std::cout << "WARNING: Trying to invert a matrix with a zero determinant\n";
		m = AnimMat4();
		return;
	}

	m = adjugate(m) * (1.0f / det);
}

AnimMat4 frustum(float l, float r, float b, float t, float n, float f) {
	if (l == r || t == b || n == f) {
		std::cout << "WARNING: Trying to create invalid frustum\n";
		return AnimMat4(); // Error
	}
	return AnimMat4(
		(2.0f * n) / (r - l), 0, 0, 0,
		0, (2.0f * n) / (t - b), 0, 0,
		(r + l) / (r - l), (t + b) / (t - b), (-(f + n)) / (f - n), -1,
		0, 0, (-2 * f * n) / (f - n), 0
	);
}

AnimMat4 perspective(float fov, float aspect, float znear, float zfar) {
	float ymax = znear * tanf(fov * 3.14159265359f / 360.0f);
	float xmax = ymax * aspect;

	return frustum(-xmax, xmax, -ymax, ymax, znear, zfar);
}

AnimMat4 ortho(float l, float r, float b, float t, float n, float f) {
	if (l == r || t == b || n == f) {
		return AnimMat4(); // Error
	}
	return AnimMat4(
		2.0f / (r - l), 0, 0, 0,
		0, 2.0f / (t - b), 0, 0,
		0, 0, -2.0f / (f - n), 0,
		-((r + l) / (r - l)), -((t + b) / (t - b)), -((f + n) / (f - n)), 1
	);
}

AnimMat4 lookAt(const AnimVec3& position, const AnimVec3& target, const AnimVec3& up) {
	// Remember, forward is negative z
	AnimVec3 f = normalized(target - position) * -1.0f;
	AnimVec3 r = cross(up, f); // Right handed
	if (r == AnimVec3(0, 0, 0)) {
		return AnimMat4(); // Error
	}
	normalize(r);
	AnimVec3 u = normalized(cross(f, r)); // Right handed

	AnimVec3 t = AnimVec3(
		-dot(r, position),
		-dot(u, position),
		-dot(f, position)
	);

	return AnimMat4(
		// Transpose upper 3x3 matrix to invert it
		r.x, u.x, f.x, 0,
		r.y, u.y, f.y, 0,
		r.z, u.z, f.z, 0,
		t.x, t.y, t.z, 1
	);
}
