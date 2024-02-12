#include "ShapeNode.h"
#include "DebugRenderer.h"

ShapeNode::ShapeNode(const Shape& shape) : SceneNode(), shape(shape), m_drawDebug(true),
localBoundingBox(shape.getAABB()) {
	OnBoundingBoxChanged();
}

void ShapeNode::OnWorldBoundingBoxUpdate() const {
	m_worldBoundingBox = getLocalBoundingBox().transformed(getWorldTransformation());
}

void ShapeNode::OnRenderOBB(const Vector4f& color) {
	if (!m_drawDebug)
		return;

	DebugRenderer::Get().AddBoundingBox(getLocalBoundingBox(), getWorldTransformation(), color);
}

void ShapeNode::OnRenderAABB(const Vector4f& color) {
	if (!m_drawDebug)
		return;

	DebugRenderer::Get().AddBoundingBox(getWorldBoundingBox(), color);
}

void ShapeNode::OnTransformChanged() {
	SceneNode::OnTransformChanged();
	OnBoundingBoxChanged();
}

void ShapeNode::OnBoundingBoxChanged() const {
	m_worldBoundingBoxDirty = true;
}

void ShapeNode::addChild(ShapeNode* node, bool drawDebug) {
	SceneNode::addChild(node);
	node->setDrawDebug(drawDebug);
}

const BoundingBox& ShapeNode::getWorldBoundingBox() const {
	if (m_worldBoundingBoxDirty) {
		OnWorldBoundingBoxUpdate();
		m_worldBoundingBoxDirty = false;
	}
	return m_worldBoundingBox;
}

const BoundingBox& ShapeNode::getLocalBoundingBox() const {
	return localBoundingBox;
}

void ShapeNode::setDrawDebug(bool drawDebug) {
	m_drawDebug = drawDebug;
}