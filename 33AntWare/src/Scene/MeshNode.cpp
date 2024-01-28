#include <Objects/MeshNew.h>

#include "MeshNode.h"
#include "DebugRenderer.h"

MeshNode::MeshNode() : SceneNode(), meshPtr(nullptr), m_model(nullptr) {
	OnBoundingBoxChanged();
}

MeshNode::MeshNode(std::shared_ptr<aw::Mesh> mesh) : SceneNode(), meshPtr(mesh), m_model(nullptr) {
	OnBoundingBoxChanged();
}

MeshNode::MeshNode(AssimpModel* model) : SceneNode(), meshPtr(nullptr), m_model(model) {
	OnBoundingBoxChanged();
}

MeshNode::MeshNode(std::shared_ptr<aw::Mesh> mesh, AssimpModel* model) : SceneNode(), meshPtr(mesh), m_model(model) {
	OnBoundingBoxChanged();
}

void MeshNode::OnWorldBoundingBoxUpdate() const{

	if (m_model) {
		worldBoundingBox = m_model->getAABB().transformed(getTransformation());
	}else if (m_mesh) {		
		worldBoundingBox = m_mesh->getLocalBoundingBox().transformed(getTransformation());
	}else {
		worldBoundingBox.define(m_position);
	}
}

void MeshNode::OnRenderDebug(){
	DebugRenderer::Get().AddBoundingBox(getWorldBoundingBox(), Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
	DebugRenderer::Get().AddBoundingBox(getLocalBoundingBox(), getTransformation(), Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
}

void MeshNode::OnTransformChanged(){
	SceneNode::OnTransformChanged();
	m_worldBoundingBoxDirty = true;
}

void MeshNode::OnBoundingBoxChanged() const{
	m_worldBoundingBoxDirty = true;
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

void MeshNode::setMesh(MeshNew* mesh) {
	m_mesh = mesh;
	OnBoundingBoxChanged();
}

void MeshNode::setModel(AssimpModel* model) {
	m_model = model;
	OnBoundingBoxChanged();
}