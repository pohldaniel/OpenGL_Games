#include "Bullet.h"

Bullet::Bullet(const MeshSequence& meshSequence, const Vector3f& projectVec) : Object(), meshSequence(meshSequence), m_meshIndex(49), m_textureIndex(5), m_materialIndex(0), m_lifeSpan(5.0f), m_speed(40.0f){
	Vector3f directon = Vector3f::Normalize(projectVec) * m_speed;
	m_rigidbody.velocity[0] = directon[0];
	m_rigidbody.velocity[1] = directon[1];
	m_rigidbody.velocity[2] = directon[2];
} 

Bullet::Bullet(Bullet const& rhs) : Object(rhs), meshSequence(rhs.meshSequence) {
	m_lifeSpan = rhs.m_lifeSpan;
	m_speed = rhs.m_speed;
	m_rigidbody = rhs.m_rigidbody;
	m_textureIndex = rhs.m_textureIndex;
	m_materialIndex = rhs.m_materialIndex;	
	m_meshIndex = rhs.m_meshIndex;
}

Bullet::Bullet(Bullet&& rhs) : Object(rhs), meshSequence(rhs.meshSequence) {
	m_lifeSpan = rhs.m_lifeSpan;
	m_speed = rhs.m_speed;
	m_rigidbody = rhs.m_rigidbody;
	m_textureIndex = rhs.m_textureIndex;
	m_materialIndex = rhs.m_materialIndex;
	m_meshIndex = rhs.m_meshIndex;
}

Bullet& Bullet::operator=(const Bullet& rhs) {
	Object::operator=(rhs);
	m_lifeSpan = rhs.m_lifeSpan;
	m_speed = rhs.m_speed;
	m_rigidbody = rhs.m_rigidbody;
	m_textureIndex = rhs.m_textureIndex;
	m_materialIndex = rhs.m_materialIndex;
	m_meshIndex = rhs.m_meshIndex;
	meshSequence = rhs.meshSequence;
	return *this;
}

Bullet& Bullet::operator=(Bullet&& rhs) {
	Object::operator=(rhs);
	m_lifeSpan = rhs.m_lifeSpan;
	m_speed = rhs.m_speed;
	m_rigidbody = rhs.m_rigidbody;
	m_textureIndex = rhs.m_textureIndex;
	m_materialIndex = rhs.m_materialIndex;
	m_meshIndex = rhs.m_meshIndex;
	meshSequence = rhs.meshSequence;
	return *this;
}

void Bullet::start() {
	m_timer.reset();
}

void Bullet::draw() {
	meshSequence.get().draw(m_meshIndex, m_textureIndex, m_materialIndex);
}

void Bullet::update(const float dt) {

}

void Bullet::fixedUpdate(float fdt) {
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

bool Bullet::timeOut() {
	if (m_timer.getElapsedTimeSec() >= m_lifeSpan){
		return true;
	}
	return false;
}