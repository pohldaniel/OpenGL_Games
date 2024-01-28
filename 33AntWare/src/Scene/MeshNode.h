#pragma once
#include <memory>
#include <engine/BoundingBox.h>
#include <engine/AssimpModel.h>
#include <Scene/SceneNode.h>

#include "Mesh.h"

class MeshNew;
class MeshNode : public SceneNode {

public:

	MeshNode();
	MeshNode(std::shared_ptr<aw::Mesh> mesh);
	MeshNode(AssimpModel* model);
	MeshNode(std::shared_ptr<aw::Mesh> mesh, AssimpModel* model);

	void OnTransformChanged() override;

	void OnWorldBoundingBoxUpdate() const;
	void OnBoundingBoxChanged() const;
	void OnRenderDebug();

	const BoundingBox& getWorldBoundingBox() const;
	const BoundingBox& getLocalBoundingBox() const;

	void setMesh(MeshNew* mesh);
	void setModel(AssimpModel* model);

	MeshNew* m_mesh;
	AssimpModel* m_model;

	std::shared_ptr<aw::Mesh> meshPtr;

	mutable BoundingBox worldBoundingBox;

	mutable bool m_worldBoundingBoxDirty;
};