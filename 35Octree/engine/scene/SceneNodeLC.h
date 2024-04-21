#pragma once

#include "BaseNode.h"

class SceneNodeLC : public BaseNode {

public:

	SceneNodeLC();
	SceneNodeLC(const SceneNodeLC& rhs);
	SceneNodeLC& operator=(const SceneNodeLC& rhs);
	SceneNodeLC(SceneNodeLC&& rhs);
	SceneNodeLC& operator=(SceneNodeLC&& rhs);

	const Matrix4f& getWorldTransformation() const override;
	const Vector3f& getWorldPosition() const override;
	const Vector3f& getWorldScale() const override;
	const Quaternion& getWorldOrientation() const override;
	void updateSOP() const;

private:

	mutable Matrix4f m_modelMatrix;

	static Vector3f WorldPosition;
	static Vector3f WorldScale;
	static Quaternion WorldOrientation;
};