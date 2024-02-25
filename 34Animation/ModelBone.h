#pragma once

#include <engine/BoundingBox.h>
#include "StringHash.h"

struct ModelBone {

	ModelBone();
	~ModelBone();
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