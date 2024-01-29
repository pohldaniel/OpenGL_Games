#include "Entity.h"
#include "Globals.h"

Entity::Entity(AssimpModel* model) : MeshNode(model), m_isStatic(false) {

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

	Vector3f appliedVelocity = Quaternion::Rotate(m_orientation, Vector3f(rigidbody.velocity.x, rigidbody.velocity.y, rigidbody.velocity.z));
	Vector3f appliedAngularVelocity = Quaternion::Rotate(m_orientation, Vector3f(rigidbody.angularVelocity.x, rigidbody.angularVelocity.y, rigidbody.angularVelocity.z));

	if (rigidbody.isLinearLocked(aw::AXIS::x))
		appliedVelocity[0] = 0;
	if (rigidbody.isLinearLocked(aw::AXIS::y))
		appliedVelocity[1] = 0;
	if (rigidbody.isLinearLocked(aw::AXIS::z))
		appliedVelocity[2] = 0;
	if (rigidbody.isAngularLocked(aw::AXIS::x))
		appliedAngularVelocity[0] = 0;
	if (rigidbody.isAngularLocked(aw::AXIS::y))
		appliedAngularVelocity[1] = 0;
	if (rigidbody.isAngularLocked(aw::AXIS::z))
		appliedAngularVelocity[2] = 0;

	translate(appliedVelocity * fdt);
	rotate(appliedAngularVelocity * fdt);
}

const Material& Entity::getMaterial() const{
	return m_model->getMeshes()[0]->getMaterial();
}