#include "ShapeEntity.h"

ShapeEntity::ShapeEntity(const Shape& shape, const Camera& camera) : ShapeNode(shape), camera(camera), m_materialIndex(-1), m_textureIndex(-1), shader(nullptr), m_disabled(false){

}

ShapeEntity::~ShapeEntity() {

}

void ShapeEntity::drawRaw() const {
	if (m_disabled) return;
	shader->use();
	shader->loadMatrix("u_model", getWorldTransformation());
	if (m_materialIndex >= 0)
		Material::GetMaterials()[m_materialIndex].bind();

	if (m_textureIndex >= 0)
		Material::GetTextures()[m_textureIndex].bind();

	shape.drawRaw();
}


const Material& ShapeEntity::getMaterial() const {
	return Material::GetMaterials()[m_materialIndex];
}

const Texture& ShapeEntity::getTexture() const {
	return Material::GetTextures()[m_textureIndex];
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

void ShapeEntity::setShader(Shader* _shader) {
	shader = _shader;
}

void ShapeEntity::setDisabled(bool disabled){
	m_disabled = disabled;
}