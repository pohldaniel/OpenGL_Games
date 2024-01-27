#pragma once

#include "SceneNode.h"
#include "BoundingBoxNew.h"

class MeshNew;
class MeshNode : public SceneNode {

	void OnWorldBoundingBoxUpdate() const;

	MeshNew* m_mesh;
	mutable BoundingBoxNew worldBoundingBox;
};