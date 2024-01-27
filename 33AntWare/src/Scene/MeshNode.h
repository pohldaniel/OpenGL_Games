#pragma once

#include "SceneNode.h"
#include "BoundingBoxNew.h"

class MeshNew;
class MeshNode : public SceneNode {

	MeshNode();

	void OnWorldBoundingBoxUpdate() const;
	void MeshNode::OnRenderDebug();

	const BoundingBoxNew& getWorldBoundingBox() const;

	void setMesh(MeshNew* mesh);

	MeshNew* m_mesh;
	mutable BoundingBoxNew worldBoundingBox;

	mutable bool m_worldBoundingBoxDirty;
};