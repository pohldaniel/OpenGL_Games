#include "MeshEntity.h"

MeshEntity::MeshEntity(const AssimpModel& model) : MeshNode(model), Entity() {

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