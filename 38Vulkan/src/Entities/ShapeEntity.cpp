#include "ShapeEntity.h"

ShapeEntity::ShapeEntity(const Shape& shape) : ShapeNode(shape), Entity() {

}

ShapeEntity::~ShapeEntity() {

}

void ShapeEntity::draw() {

	shader->use();
	shader->loadMatrix("modelMatrix", getWorldTransformation());

	if (m_materialIndex >= 0)
		Material::GetMaterials()[m_materialIndex].bind();

	if (m_textureIndex >= 0)
		Material::GetTextures()[m_textureIndex].bind();

	shape.drawRaw();
}