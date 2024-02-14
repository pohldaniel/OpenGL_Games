#pragma once
#include <memory>
#include <engine/scene/SceneNode.h>
#include <engine/scene/sceneNodeLC.h>
#include <engine/BoundingBox.h>
#include <engine/AssimpModel.h>

class MeshNode : public SceneNode {

public:

	MeshNode();
	MeshNode(AssimpModel* model);
	~MeshNode();

	void OnTransformChanged() override;

	void OnWorldBoundingBoxUpdate() const;
	void OnBoundingBoxChanged() const;
	void OnRenderOBB(const Vector4f& color = {1.0f, 0.0f, 0.0f, 1.0f});
	void OnRenderAABB(const Vector4f& color = { 0.0f, 1.0f, 0.0f, 1.0f });

	using SceneNode::addChild;
	void addChild(MeshNode* node, bool drawDebug);

	const BoundingBox& getWorldBoundingBox() const;
	const BoundingBox& getLocalBoundingBox() const;

	void setModel(AssimpModel* model);
	AssimpModel* getModel() const;

protected:

	AssimpModel* m_model;

private:

	mutable BoundingBox worldBoundingBox;
	mutable bool m_worldBoundingBoxDirty;
};