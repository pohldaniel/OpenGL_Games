#include "Entity.h"
#include "Globals.h"

Entity::Entity(AssimpModel* model) : MeshNode(model), m_isStatic(false), m_markForDelete(false){

}

void Entity::draw(const Camera& camera) {	
	if(m_model)
		m_model->drawRaw();
}

void Entity::update(const float dt) {

}

void Entity::fixedUpdate(float fdt) {
	if (m_isStatic)
		return;

	rigidbody.velocity += rigidbody.acceleration * fdt;
	rigidbody.angularVelocity += rigidbody.angularAcceleration * fdt;

	Vector3f appliedVelocity = Quaternion::Rotate(m_orientation, Vector3f(rigidbody.velocity[0], rigidbody.velocity[1], rigidbody.velocity[2]));
	Vector3f appliedAngularVelocity = Quaternion::Rotate(m_orientation, Vector3f(rigidbody.angularVelocity[0], rigidbody.angularVelocity[1], rigidbody.angularVelocity[2]));

	if (rigidbody.isLinearLocked(AXIS::x))
		appliedVelocity[0] = 0.0f;
	if (rigidbody.isLinearLocked(AXIS::y))
		appliedVelocity[1] = 0.0f;
	if (rigidbody.isLinearLocked(AXIS::z))
		appliedVelocity[2] = 0.0f;
	if (rigidbody.isAngularLocked(AXIS::x))
		appliedAngularVelocity[0] = 0.0f;
	if (rigidbody.isAngularLocked(AXIS::y))
		appliedAngularVelocity[1] = 0.0f;
	if (rigidbody.isAngularLocked(AXIS::z))
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