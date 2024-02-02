#pragma once

#include "ObjectNode.h"

class SceneNode : public ObjectNode {

public:

	SceneNode();
	const Matrix4f& getTransformation() const override;

	const Vector3f& getPosition() const override;
	const Vector3f& getScale() const override;
	const Quaternion& getOrientation() const override;

private:

	mutable Matrix4f m_modelMatrix;
};