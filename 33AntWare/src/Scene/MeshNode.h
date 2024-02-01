#pragma once
#include <memory>
#include <engine/BoundingBox.h>
#include <engine/AssimpModel.h>
#include <Scene/SceneNode.h>

class MeshNode : public SceneNode {

public:

	MeshNode();
	MeshNode(AssimpModel* model);

	void OnTransformChanged() override;

	void OnWorldBoundingBoxUpdate() const;
	void OnBoundingBoxChanged() const;
	void OnRenderOBB(const Vector4f& color = {1.0f, 0.0f, 0.0f, 1.0f});
	void OnRenderAABB(const Vector4f& color = { 0.0f, 1.0f, 0.0f, 1.0f });

	using SceneNode::addChild;
	void addChild(MeshNode* node, bool drawDebug);
	//void addChild(SceneNode* node);

	const BoundingBox& getWorldBoundingBox() const;
	const BoundingBox& getLocalBoundingBox() const;

	void setModel(AssimpModel* model);
	void setDrawDebug(bool drawDebug);

	AssimpModel* m_model;

	mutable BoundingBox worldBoundingBox;
	mutable bool m_worldBoundingBoxDirty;
	bool m_drawDebug;
};