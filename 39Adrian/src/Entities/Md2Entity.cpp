#include <Physics/ShapeDrawer.h>
#include "Md2Entity.h"

Md2Entity::Md2Entity(const Md2Model& md2Model) : Md2Node(md2Model), Entity(), m_isActive(false), m_rigidBody(nullptr), m_color(Vector4f::ONE){

}

Md2Entity::~Md2Entity() {

}

void Md2Entity::drawRaw() const {
	Md2Node::drawRaw();
}

void Md2Entity::draw() {
	Md2Node::drawRaw();
}

void Md2Entity::update(const float dt) {
	Md2Node::update(dt);
}

void Md2Entity::fixedUpdate(float fdt) {
	const BoundingBox& aabb = getLocalBoundingBox();
	const Vector3f& pos = getWorldPosition();
	const Vector3f size = aabb.getSize();

	m_rigidBody->setWorldTransform(Physics::BtTransform(aabb.min + pos + 0.5f * size));
	m_rigidBody->getCollisionShape()->setLocalScaling(Physics::VectorFrom(size));
	//Physics::GetDynamicsWorld()->updateSingleAabb(m_rigidBody);
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
	m_color = m_isActive ? Vector4f(0.7f, 0.7f, 1.0f, 1.0f) : Vector4f::ONE;
}

const Vector4f& Md2Entity::getColor() const {
	return m_color;
}

bool Md2Entity::isActive() {
	return m_isActive;
}

void Md2Entity::setRigidBody(btRigidBody* rigidBody) {
	m_rigidBody = rigidBody;
	ShapeDrawer::Get().addToCache(m_rigidBody->getCollisionShape());
}

btRigidBody* Md2Entity::getRigidBody() {
	return m_rigidBody;
}

void Md2Entity::move(float x, float z) {
	setPosition(x, MAP_MODEL_HEIGHT_Y, z);
}