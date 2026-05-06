#pragma once

#include <string>
#include <engine/Vector.h>

struct MeshBone {

	MeshBone();
	~MeshBone();
	std::string name;
	Vector3f initialPosition;
	Quaternion initialRotation;
	Vector3f initialScale;
	Matrix4f offsetMatrix;
	float radius;
	size_t parentIndex;
	bool active;

	void scale(const float sx, const float sy, const float sz);
	void rotate(const float pitch, const float yaw, const float roll);
};