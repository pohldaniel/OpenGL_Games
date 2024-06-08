#include "MeshEntity.h"

MeshEntity::MeshEntity(const AssimpModel& model) : MeshNode(model), m_materialIndex(-1), m_textureIndex(-1) {

}

MeshEntity::~MeshEntity() {

}

void MeshEntity::draw() {

	if (m_materialIndex >= 0)
		Material::GetMaterials()[m_materialIndex].bind();

	if (m_textureIndex >= 0)
		Material::GetTextures()[m_textureIndex].bind();

	model.drawRaw();
}

void MeshEntity::update(const float dt) {

}

const Material& MeshEntity::getMaterial() const {
	return Material::GetMaterials()[m_materialIndex];
}

short MeshEntity::getMaterialIndex() const {
	return m_materialIndex;
}

void MeshEntity::setMaterialIndex(short index) const {
	m_materialIndex = index;
}

short MeshEntity::getTextureIndex() const {
	return m_textureIndex;
}

void MeshEntity::setTextureIndex(short index) const {
	m_textureIndex = index;
}