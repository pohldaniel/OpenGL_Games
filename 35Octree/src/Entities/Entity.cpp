#include "Entity.h"
#include "Globals.h"

Entity::Entity(const AssimpModel& model) : MeshNode(model), m_materialIndex(-1) {

}

Entity::~Entity() {

}

void Entity::draw() {
	
	if(m_materialIndex >= 0)
		Material::GetMaterials()[m_materialIndex].bind();

	model.drawRaw();
}

void Entity::update(const float dt) {

}


const Material& Entity::getMaterial() const {
	return Material::GetMaterials()[m_materialIndex];
}

short Entity::getMaterialIndex() const {
	return m_materialIndex;
}

void Entity::setMaterialIndex(short index) const {
	m_materialIndex = index;
}