#pragma once

#include <string>
#include <engine/Vector.h>

struct BoneDescription {
	BoneDescription();
	~BoneDescription();
	std::string name;
	Vector3f initialPosition;
	Quaternion initialRotation;
	Vector3f initialScale;
	Matrix4f offsetMatrix;
	size_t parentIndex;
	float radius;
};