#include "Md2Entity.h"

Md2Entity::Md2Entity(const Md2Model& md2Model) : Md2Node(md2Model), Entity(), m_isActive(false){

}

Md2Entity::~Md2Entity() {

}

void Md2Entity::draw() {	
	Md2Node::drawRaw();
}

void Md2Entity::fixedUpdate(float fdt) {

}

void Md2Entity::update(const float dt) {
	Md2Node::update(dt);
}

short Md2Entity::getMaterialIndex() const {
	return Md2Node::m_materialIndex;
}

void Md2Entity::setMaterialIndex(short index) {
	Md2Node::m_materialIndex = index;
}

short Md2Entity::getTextureIndex() const {
	return Md2Node::m_textureIndex;
}

void Md2Entity::setTextureIndex(short index) {
	Md2Node::m_textureIndex = index;
}

void Md2Entity::setIsActive(bool isActive) {
	m_isActive = isActive;
}

bool Md2Entity::isActive() {
	return m_isActive;
}