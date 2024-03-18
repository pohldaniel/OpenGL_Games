#pragma once
#include <memory>
#include "SceneNode.h"
#include "sceneNodeLC.h"
#include "../BoundingBox.h"
#include "../AssimpModel.h"

class MeshNode : public SceneNodeLC {

public:

	MeshNode(const AssimpModel& model);
	~MeshNode() = default;

	void OnTransformChanged() override;

	void OnWorldBoundingBoxUpdate() const;
	void OnBoundingBoxChanged() const;
	void OnRenderOBB(const Vector4f& color = {1.0f, 0.0f, 0.0f, 1.0f});
	void OnRenderAABB(const Vector4f& color = { 0.0f, 1.0f, 0.0f, 1.0f });

	using SceneNodeLC::addChild;
	void addChild(MeshNode* node, bool drawDebug);

	const BoundingBox& getWorldBoundingBox() const;
	const BoundingBox& getLocalBoundingBox() const;
	void setDrawDebug(bool drawDebug);

protected:

	const AssimpModel& model;

private:

	mutable BoundingBox m_worldBoundingBox;
	mutable bool m_worldBoundingBoxDirty;
	bool m_drawDebug;

	const BoundingBox& localBoundingBox;
};