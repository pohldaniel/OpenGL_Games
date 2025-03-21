#pragma once

#include "SceneNode.h"
#include "SceneNodeLC.h"
#include "../BoundingBox.h"
#include "../MeshSequence.h"

class SequenceNode : public SceneNode {

public:

	SequenceNode(const MeshSequence& meshSequence, int meshIndex);

	void OnTransformChanged() override;

	void OnWorldBoundingBoxUpdate() const;
	void OnBoundingBoxChanged() const;
	void OnRenderOBB(const Vector4f& color = { 1.0f, 0.0f, 0.0f, 1.0f });
	void OnRenderAABB(const Vector4f& color = { 0.0f, 1.0f, 0.0f, 1.0f });

	using SceneNode::addChild;
	void addChild(SequenceNode* node, bool drawDebug);

	const BoundingBox& getWorldBoundingBox() const;
	const BoundingBox& getLocalBoundingBox() const;

	int getMeshIndex() const;

protected:

	const MeshSequence& meshSequence;
	mutable int m_meshIndex;

private:

	mutable BoundingBox m_worldBoundingBox;
	mutable bool m_worldBoundingBoxDirty;
	const BoundingBox& localBoundingBox;
};