#pragma once

#include <engine/MeshObject/Shape.h>
#include <engine/scene/SceneNode.h>
#include <engine/BoundingBox.h>

class ShapeNode : public SceneNode {

public:

	ShapeNode(const Shape& shape);

	void OnTransformChanged() override;

	void OnWorldBoundingBoxUpdate() const;
	void OnBoundingBoxChanged() const;
	void OnRenderOBB(const Vector4f& color = { 1.0f, 0.0f, 0.0f, 1.0f });
	void OnRenderAABB(const Vector4f& color = { 0.0f, 1.0f, 0.0f, 1.0f });

	using SceneNode::addChild;
	void addChild(ShapeNode* node, bool drawDebug);

	const BoundingBox& getWorldBoundingBox() const;
	const BoundingBox& getLocalBoundingBox() const;
	void setDrawDebug(bool drawDebug);

protected:

	const Shape& shape;

private:

	mutable BoundingBox m_worldBoundingBox;
	mutable bool m_worldBoundingBoxDirty;
	bool m_drawDebug;

	const BoundingBox& localBoundingBox;

};