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
