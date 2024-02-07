#include "SequenceNode.h"
#include "DebugRenderer.h"

SequenceNode::SequenceNode(const MeshSequence& meshSequence, int meshIndex) : SceneNode(), meshSequence(meshSequence), m_meshIndex(meshIndex), m_drawDebug(true),
localBoundingBox(meshSequence.getMeshes()[m_meshIndex].localBoundingBox) {
	OnBoundingBoxChanged();
}

void SequenceNode::OnWorldBoundingBoxUpdate() const {
	m_worldBoundingBox = getLocalBoundingBox().transformed(getWorldTransformation());
}

void SequenceNode::OnRenderOBB(const Vector4f& color) {
	if (!m_drawDebug)
		return;

	DebugRenderer::Get().AddBoundingBox(getLocalBoundingBox(), getWorldTransformation(), color);
}

void SequenceNode::OnRenderAABB(const Vector4f& color) {
	if (!m_drawDebug)
		return;

	DebugRenderer::Get().AddBoundingBox(getWorldBoundingBox(), color);
}

void SequenceNode::OnTransformChanged() {
	SceneNode::OnTransformChanged();
	OnBoundingBoxChanged();
}

void SequenceNode::OnBoundingBoxChanged() const {
	m_worldBoundingBoxDirty = true;
}

void SequenceNode::addChild(SequenceNode* node, bool drawDebug) {
	SceneNode::addChild(node);
	node->setDrawDebug(drawDebug);
}

const BoundingBox& SequenceNode::getWorldBoundingBox() const {
	if (m_worldBoundingBoxDirty) {
		OnWorldBoundingBoxUpdate();
		m_worldBoundingBoxDirty = false;
	}
	return m_worldBoundingBox;
}

const BoundingBox& SequenceNode::getLocalBoundingBox() const {
	return localBoundingBox;
}

void SequenceNode::setDrawDebug(bool drawDebug) {
	m_drawDebug = drawDebug;
}

int SequenceNode::getMeshIndex() const {
	return m_meshIndex;
}