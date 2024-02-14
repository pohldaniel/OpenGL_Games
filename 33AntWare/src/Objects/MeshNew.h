#pragma once

#include <engine/scene/MeshNode.h>

class MeshNew {

public:

	void setOwner(SceneNode* owner);
	SceneNode* getOwner() const;

	const Vector3f& getPosition() const;
	const Quaternion& getdOrientation() const;
	const Vector3f& getScale() const;

	const BoundingBox& getLocalBoundingBox() const;
	void setLocalBoundingBox(const BoundingBox& box);

private:

	SceneNode* owner;
	Matrix4f* worldTransform;	
	BoundingBox localBoundingBox;
};