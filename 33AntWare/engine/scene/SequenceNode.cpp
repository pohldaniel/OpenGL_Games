#include "SequenceNode.h"
#include "../DebugRenderer.h"

SequenceNode::SequenceNode(const MeshSequence& meshSequence, int meshIndex) : SceneNode(), meshSequence(meshSequence), m_meshIndex(meshIndex),
localBoundingBox(meshSequence.getLocalBoundingBox(m_meshIndex)) {
	OnBoundingBoxChanged();
}

void SequenceNode::OnWorldBoundingBoxUpdate() const {
	m_worldBoundingBox = getLocalBoundingBox().transformed(getWorldTransformation());
}

void SequenceNode::OnRenderOBB(const Vector4f& color) {
	DebugRenderer::Get().AddBoundingBox(getLocalBoundingBox(), getWorldTransformation(), color);
}

void SequenceNode::OnRenderAABB(const Vector4f& color) {
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

int SequenceNode::getMeshIndex() const {
	return m_meshIndex;
}