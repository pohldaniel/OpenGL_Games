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
	void OnRenderOBB(const Vector4f& color = {1.0f, 0.0f, 0.0f, 1.0f});
	void OnRenderAABB(const Vector4f& color = { 0.0f, 1.0f, 0.0f, 1.0f });

	using SceneNode::addChild;
	void addChild(MeshNode* node, bool drawDebug);

	const BoundingBox& getWorldBoundingBox() const;
	const BoundingBox& getLocalBoundingBox() const;

	void setMesh(MeshNew* mesh);
	void setModel(AssimpModel* model);
	void setDrawDebug(bool drawDebug);

	MeshNew* m_mesh;
	AssimpModel* m_model;

	std::shared_ptr<aw::Mesh> meshPtr;

	mutable BoundingBox worldBoundingBox;
	mutable bool m_worldBoundingBoxDirty;
	bool m_drawDebug;
};