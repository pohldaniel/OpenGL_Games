#pragma once
#include <engine/BoundingBox.h>
#include <Scene/SceneNode.h>
#include "ObjSequence.h"

class SequenceNode : public SceneNode {

public:

	SequenceNode(const ObjSequence& objSequence);

	void OnTransformChanged() override;

	void OnWorldBoundingBoxUpdate() const;
	void OnBoundingBoxChanged() const;
	void OnRenderOBB(const Vector4f& color = { 1.0f, 0.0f, 0.0f, 1.0f });
	void OnRenderAABB(const Vector4f& color = { 0.0f, 1.0f, 0.0f, 1.0f });

	using SceneNode::addChild;
	void addChild(SequenceNode* node, bool drawDebug);

	const BoundingBox& getWorldBoundingBox() const;
	const BoundingBox& getLocalBoundingBox() const;

private:

	void setDrawDebug(bool drawDebug);

	bool m_drawDebug;
	int m_meshIndex;
	mutable BoundingBox m_worldBoundingBox;
	mutable bool m_worldBoundingBoxDirty;

	const BoundingBox& localBoundingBox;
	const ObjSequence& objSequence;
};