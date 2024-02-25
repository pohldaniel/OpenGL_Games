#include "ModelBone.h"

ModelBone::ModelBone() :
	initialPosition(Vector3f::ZERO),
	initialRotation(Quaternion::IDENTITY),
	initialScale(Vector3f::ONE),
	offsetMatrix(Matrix4f::IDENTITY),
	radius(0.0f),
	boundingBox(0.0f, 0.0f),
	parentIndex(0),
	active(true)
{
}

ModelBone::~ModelBone()
{
}

void ModelBone::scale(const float sx, const float sy, const float sz) {
	offsetMatrix *= Matrix4f::Scale(sx, sy, sz);
}

void ModelBone::rotate(const float pitch, const float yaw, const float roll) {
	offsetMatrix *= Matrix4f::Rotate(pitch, yaw, roll);
}