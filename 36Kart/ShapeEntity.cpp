#include "ShapeEntity.h"

ShapeEntity::ShapeEntity(const Shape& shape) : ShapeNode(shape), m_materialIndex(-1), m_textureIndex(-1) {

}

ShapeEntity::~ShapeEntity() {

}

void ShapeEntity::draw() {

	if (m_materialIndex >= 0)
		Material::GetMaterials()[m_materialIndex].bind();

	if (m_textureIndex >= 0)
		Material::GetTextures()[m_textureIndex].bind();

	shape.drawRaw();
}

void ShapeEntity::update(const float dt) {

}

const Material& ShapeEntity::getMaterial() const {
	return Material::GetMaterials()[m_materialIndex];
}

short ShapeEntity::getMaterialIndex() const {
	return m_materialIndex;
}

void ShapeEntity::setMaterialIndex(short index) const {
	m_materialIndex = index;
}

short ShapeEntity::getTextureIndex() const {
	return m_textureIndex;
}

void ShapeEntity::setTextureIndex(short index) const {
	m_textureIndex = index;
}