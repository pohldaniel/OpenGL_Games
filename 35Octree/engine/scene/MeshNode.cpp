#include "MeshNode.h"
#include "../DebugRenderer.h"

MeshNode::MeshNode(const AssimpModel& model) : SceneNodeLC(), model(model), m_drawDebug(true), localBoundingBox(model.getAABB()) {
	OnBoundingBoxChanged();
}

void MeshNode::OnWorldBoundingBoxUpdate() const{
	m_worldBoundingBox = getLocalBoundingBox().transformed(getWorldTransformation());
}

void MeshNode::OnRenderOBB(const Vector4f& color){
	DebugRenderer::Get().AddBoundingBox(getLocalBoundingBox(), getWorldTransformation(), color);
}

void MeshNode::OnRenderAABB(const Vector4f& color) {
	DebugRenderer::Get().AddBoundingBox(getWorldBoundingBox(), color);
}

void MeshNode::OnTransformChanged(){
	SceneNodeLC::OnTransformChanged();
	OnBoundingBoxChanged();
}

void MeshNode::OnBoundingBoxChanged() const{
	m_worldBoundingBoxDirty = true;
}

void MeshNode::addChild(MeshNode* node, bool drawDebug) {
	SceneNodeLC::addChild(node);
}

const BoundingBox& MeshNode::getWorldBoundingBox() const {
	if (m_worldBoundingBoxDirty){
		OnWorldBoundingBoxUpdate();
		m_worldBoundingBoxDirty = false;
	}
	return m_worldBoundingBox;
}

const BoundingBox& MeshNode::getLocalBoundingBox() const {
	return model.getAABB();
}

void MeshNode::setDrawDebug(bool drawDebug) {
	m_drawDebug = drawDebug;
}