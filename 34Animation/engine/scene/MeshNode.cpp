#include "MeshNode.h"
#include "../DebugRenderer.h"

MeshNode::MeshNode() : SceneNode(), m_model(nullptr){
	OnBoundingBoxChanged();
}

MeshNode::MeshNode(AssimpModel* model) : SceneNode(), m_model(model){
	OnBoundingBoxChanged();
}

MeshNode::~MeshNode() {
	delete m_model;
}

void MeshNode::OnWorldBoundingBoxUpdate() const{

	if (m_model) {
		worldBoundingBox = m_model->getAABB().transformed(getWorldTransformation());
	}else {
		worldBoundingBox.define(m_position);
	}
}

void MeshNode::OnRenderOBB(const Vector4f& color){
	DebugRenderer::Get().AddBoundingBox(getLocalBoundingBox(), getWorldTransformation(), color);
}

void MeshNode::OnRenderAABB(const Vector4f& color) {
	DebugRenderer::Get().AddBoundingBox(getWorldBoundingBox(), color);
}

void MeshNode::OnTransformChanged(){
	SceneNode::OnTransformChanged();
	OnBoundingBoxChanged();
}

void MeshNode::OnBoundingBoxChanged() const{
	m_worldBoundingBoxDirty = true;
}

void MeshNode::addChild(MeshNode* node, bool drawDebug) {
	SceneNode::addChild(node);
}

const BoundingBox& MeshNode::getWorldBoundingBox() const {
	if (m_worldBoundingBoxDirty){
		OnWorldBoundingBoxUpdate();
		m_worldBoundingBoxDirty = false;
	}
	return worldBoundingBox;
}

const BoundingBox& MeshNode::getLocalBoundingBox() const {
	return m_model->getAABB();
}

void MeshNode::setModel(AssimpModel* model) {
	m_model = model;
	OnBoundingBoxChanged();
}

AssimpModel* MeshNode::getModel() const {
	return m_model;
}