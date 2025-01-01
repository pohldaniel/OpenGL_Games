#include "Md2Entity.h"

Md2Entity::Md2Entity(const Md2Model& md2Model) : Md2Node(md2Model), Entity(), m_isActive(true), m_rigidBody(nullptr){

}

Md2Entity::~Md2Entity() {

}

void Md2Entity::draw() {
	if(m_isActive)
	  Md2Node::drawRaw();
}

void Md2Entity::update(const float dt) {
	Md2Node::update(dt);
}

void Md2Entity::fixedUpdate(float fdt) {

	const BoundingBox& aabb = getLocalBoundingBox();
	const Vector3f& pos = getWorldPosition();
	const Vector3f size = aabb.getSize();

	m_rigidBody->setWorldTransform(Physics::BtTransform(aabb.min + pos + 0.5f * size ));
	m_rigidBody->getCollisionShape()->setLocalScaling(Physics::VectorFrom(size));
	Physics::GetDynamicsWorld()->updateSingleAabb(m_rigidBody);
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
