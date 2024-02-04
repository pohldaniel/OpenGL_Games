#include "Entity.h"
#include "Globals.h"

Entity::Entity(AssimpModel* model) : MeshNode(model), m_isStatic(false), m_markForDelete(false){

}

void Entity::draw() {	
	if(m_model)
		m_model->drawRaw();
}

void Entity::update(const float dt) {

}

void Entity::fixedUpdate(float fdt) {
	if (m_isStatic)
		return;

	m_rigidbody.velocity += m_rigidbody.acceleration * fdt;
	m_rigidbody.angularVelocity += m_rigidbody.angularAcceleration * fdt;

	Vector3f appliedVelocity = Quaternion::Rotate(m_orientation, Vector3f(m_rigidbody.velocity[0], m_rigidbody.velocity[1], m_rigidbody.velocity[2]));
	Vector3f appliedAngularVelocity = Quaternion::Rotate(m_orientation, Vector3f(m_rigidbody.angularVelocity[0], m_rigidbody.angularVelocity[1], m_rigidbody.angularVelocity[2]));

	if (m_rigidbody.isLinearLocked(AXIS::x))
		appliedVelocity[0] = 0.0f;
	if (m_rigidbody.isLinearLocked(AXIS::y))
		appliedVelocity[1] = 0.0f;
	if (m_rigidbody.isLinearLocked(AXIS::z))
		appliedVelocity[2] = 0.0f;
	if (m_rigidbody.isAngularLocked(AXIS::x))
		appliedAngularVelocity[0] = 0.0f;
	if (m_rigidbody.isAngularLocked(AXIS::y))
		appliedAngularVelocity[1] = 0.0f;
	if (m_rigidbody.isAngularLocked(AXIS::z))
		appliedAngularVelocity[2] = 0.0f;

	translate(appliedVelocity * fdt);
	rotate(appliedAngularVelocity * fdt);
}

const Material& Entity::getMaterial() const{
	return m_model->getMeshes()[0]->getMaterial();
}

void Entity::markForDelete() {
	m_markForDelete = true;
}

bool Entity::isMarkForDelete() {
	return m_markForDelete;
}

void Entity::setRigidbody(const Rigidbody& rigidbody) {
	m_rigidbody = rigidbody;
}

void Entity::setIsStatic(bool isStatic) {
	m_isStatic = isStatic;
}