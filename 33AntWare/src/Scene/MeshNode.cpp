#include <Objects/MeshNew.h>

#include "MeshNode.h"
#include "DebugRenderer.h"

MeshNode::MeshNode() : SceneNode(), meshPtr(nullptr), m_model(nullptr), m_drawDebug(true){
	OnBoundingBoxChanged();
}

MeshNode::MeshNode(std::shared_ptr<aw::Mesh> mesh) : SceneNode(), meshPtr(mesh), m_model(nullptr), m_drawDebug(true) {
	OnBoundingBoxChanged();
}

MeshNode::MeshNode(AssimpModel* model) : SceneNode(), meshPtr(nullptr), m_model(model), m_drawDebug(true) {
	OnBoundingBoxChanged();
}

MeshNode::MeshNode(std::shared_ptr<aw::Mesh> mesh, AssimpModel* model) : SceneNode(), meshPtr(mesh), m_model(model), m_drawDebug(true) {
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

void MeshNode::OnRenderOBB(const Vector4f& color){

	if (!m_drawDebug)
		return;

	DebugRenderer::Get().AddBoundingBox(getLocalBoundingBox(), getTransformation(), color);
}

void MeshNode::OnRenderAABB(const Vector4f& color) {
	if (!m_drawDebug)
		return;
	
	DebugRenderer::Get().AddBoundingBox(getWorldBoundingBox(), color);
}

void MeshNode::OnTransformChanged(){
	SceneNode::OnTransformChanged();
	m_worldBoundingBoxDirty = true;
}

void MeshNode::OnBoundingBoxChanged() const{
	m_worldBoundingBoxDirty = true;
}

void MeshNode::addChild(MeshNode* node, bool drawDebug) {
	SceneNode::addChild(node);
	node->setDrawDebug(drawDebug);
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

void MeshNode::setDrawDebug(bool drawDebug) {
	m_drawDebug = drawDebug;
}