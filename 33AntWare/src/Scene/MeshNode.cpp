#include <Objects/MeshNew.h>

#include "MeshNode.h"
#include "DebugRenderer.h"

MeshNode::MeshNode() : SceneNode() {
	m_worldBoundingBoxDirty = true;
}

void MeshNode::OnWorldBoundingBoxUpdate() const{
	if (m_mesh)
		worldBoundingBox = m_mesh->getLocalBoundingBox().transformed(getTransformation());
	else
		worldBoundingBox.define(m_position);
}

void MeshNode::OnRenderDebug(){
	DebugRenderer::Get().AddBoundingBox(getWorldBoundingBox(), Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
}

const BoundingBoxNew& MeshNode::getWorldBoundingBox() const {
	if (m_worldBoundingBoxDirty){
		OnWorldBoundingBoxUpdate();
		m_worldBoundingBoxDirty = false;
	}
	return worldBoundingBox;
}

void MeshNode::setMesh(MeshNew* mesh) {
	m_mesh = mesh;
	m_worldBoundingBoxDirty = true;
}