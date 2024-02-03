#include "SequenceNode.h"
#include "DebugRenderer.h"

SequenceNode::SequenceNode(const ObjSequence& objSequence) : SceneNode(), 
objSequence(objSequence), m_meshIndex(m_meshIndex),
localBoundingBox(objSequence.getMeshes()[m_meshIndex].localBoundingBox)
{
	OnBoundingBoxChanged();
}

void SequenceNode::OnWorldBoundingBoxUpdate() const {
	m_worldBoundingBox = localBoundingBox.transformed(getTransformation());
}

void SequenceNode::OnRenderOBB(const Vector4f& color) {
	if (!m_drawDebug)
		return;

	DebugRenderer::Get().AddBoundingBox(getLocalBoundingBox(), getTransformation(), color);
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