#include "Entity.h"

Entity::Entity() : m_materialIndex(-1), m_textureIndex(-1), shader(nullptr) {

}

void Entity::draw() {

}

void Entity::update(const float dt) {

}

const Material& Entity::getMaterial() const {
	return Material::GetMaterials()[m_materialIndex];
}

short Entity::getMaterialIndex() const {
	return m_materialIndex;
}

void Entity::setMaterialIndex(short index){
	m_materialIndex = index;
}

short Entity::getTextureIndex() const {
	return m_textureIndex;
}

void Entity::setTextureIndex(short index){
	m_textureIndex = index;
}

void Entity::setShader(Shader* shader) {
	Entity::shader = shader;
}