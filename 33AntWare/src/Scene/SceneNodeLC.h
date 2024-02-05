#pragma once

#include "BaseNode.h"

class SceneNodeLC : public BaseNode {

public:

	SceneNodeLC();
	const Matrix4f& getWorldTransformation() const override;

	const Vector3f& getWorldPosition() const;
	const Vector3f& getWorldScale() const;
	const Quaternion& getWorldOrientation() const;

private:

	mutable Matrix4f m_modelMatrix;

	static Vector3f WorldPosition;
	static Vector3f WorldScale;
	static Quaternion WorldOrientation;
};