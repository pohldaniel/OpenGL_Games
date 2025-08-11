#include "MeshNode.h"
#include "../DebugRenderer.h"

MeshNode::MeshNode(const AssimpModel& model) : OctreeNode(), 
	model(model),
	m_materialIndex(-1),
	m_textureIndex(-1) {
	OnBoundingBoxChanged();
}

void MeshNode::drawRaw(bool force) const {

	if (m_materialIndex >= 0)
		Material::GetMaterials()[m_materialIndex].updateMaterialUbo(BuiltInShader::materialUbo);

	if (m_textureIndex >= 0)
		Material::GetTextures()[m_textureIndex].bind();

	model.drawRaw();
}

void MeshNode::addChild(MeshNode* node, bool drawDebug) {
	OctreeNode::addChild(node);
	node->setDrawDebug(drawDebug);
}

const AssimpModel& MeshNode::getModel() const {
	return model;
}

const BoundingBox& MeshNode::getLocalBoundingBox() const {
	return model.getAABB();
}

short MeshNode::getMaterialIndex() const {
	return m_materialIndex;
}

void MeshNode::setMaterialIndex(short index) {
	m_materialIndex = index;
}

short MeshNode::getTextureIndex() const {
	return m_textureIndex;
}

void MeshNode::setTextureIndex(short index) {
	m_textureIndex = index;
}