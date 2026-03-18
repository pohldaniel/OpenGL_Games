#pragma once

#include "BoundingBox.h"
#include "utils/StringHash.h"

struct MeshBone {

	MeshBone();
	~MeshBone();
	std::string name;
	StringHash nameHash;
	Vector3f initialPosition;
	Quaternion initialRotation;
	Vector3f initialScale;
	Matrix4f offsetMatrix;
	float radius;
	BoundingBox boundingBox;
	size_t parentIndex;
	bool active;

	void scale(const float sx, const float sy, const float sz);
	void rotate(const float pitch, const float yaw, const float roll);
};