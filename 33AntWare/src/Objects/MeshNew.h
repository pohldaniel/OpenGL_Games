#pragma once
#include <engine/Vector.h>
#include <Objects/SceneNode.h>

#include "BoundingBoxNew.h"

class MeshNew {

public:

	void setOwner(SceneNode* owner);
	SceneNode* getOwner() const;

	//Vector3f getPosition() const { return WorldTransform().Translation(); }
	//Quaternion getdRotation() const { return WorldTransform().Rotation(); }
	//Vector3f getScale() const { return WorldTransform().Scale(); }

private:

	Matrix4f* worldTransform;
	SceneNode* owner;
	BoundingBoxNew boundingBox;
};