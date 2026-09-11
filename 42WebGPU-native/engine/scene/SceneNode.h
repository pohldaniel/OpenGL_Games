#pragma once

#include "BaseNode.h"

class SceneNode : public BaseNode {

public:

	SceneNode();
	SceneNode(const SceneNode& rhs);
	SceneNode& operator=(const SceneNode& rhs);
	SceneNode(SceneNode&& rhs);
	SceneNode& operator=(SceneNode&& rhs);

	const Matrix4f& getWorldTransformation() const override;
	const Vector3f& getWorldPosition(bool update = true) const override;
	const Vector3f& getWorldScale(bool update = true) const override;
	const Quaternion& getWorldOrientation(bool update = true) const override;
	void updateWorldTransformation() const;

private:

	mutable Matrix4f m_modelMatrix;
	static thread_local Vector3f WorldPosition;
	static thread_local Vector3f WorldScale;
	static thread_local Quaternion WorldOrientation;
};