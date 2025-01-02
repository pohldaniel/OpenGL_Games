#include "ShapeEntity.h"

ShapeEntity::ShapeEntity(const Shape& shape) : ShapeNode(shape), Entity() {

}

ShapeEntity::~ShapeEntity() {

}

void ShapeEntity::draw() {

	shader->use();
	shader->loadMatrix("modelMatrix", getWorldTransformation());

	ShapeNode::drawRaw();
}

short ShapeEntity::getMaterialIndex() const {
	return ShapeNode::m_materialIndex;
}

void ShapeEntity::setMaterialIndex(short index) {
	ShapeNode::m_materialIndex = index;
}

short ShapeEntity::getTextureIndex() const {
	return ShapeNode::m_textureIndex;
}

void ShapeEntity::setTextureIndex(short index) {
	ShapeNode::m_textureIndex = index;
}