#include "BoneDescription.h"

BoneDescription::BoneDescription() :
	initialPosition(Vector3f::ZERO),
	initialRotation(Quaternion::IDENTITY),
	initialScale(Vector3f::ONE),
	offsetMatrix(Matrix4f::IDENTITY),
	radius(0.0f),
	parentIndex(0) {
}

BoneDescription::~BoneDescription() {

}