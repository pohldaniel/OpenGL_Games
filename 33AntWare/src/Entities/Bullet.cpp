#include "Bullet.h"

AssimpModel* Bullet::Model = nullptr;

Bullet::Bullet(const Vector3f& projectVec) : Object(){
	Vector3f directon = Vector3f::Normalize(projectVec) * speed;
	m_rigidbody.velocity[0] = directon[0];
	m_rigidbody.velocity[1] = directon[1];
	m_rigidbody.velocity[2] = directon[2];
} 

Bullet::Bullet(Bullet const& rhs) : Object(rhs){
	m_rigidbody = rhs.m_rigidbody;
}

Bullet::Bullet(Bullet&& rhs) : Object(rhs) {
	m_rigidbody = rhs.m_rigidbody;
}

Bullet& Bullet::operator=(const Bullet& rhs) {
	Object::operator=(rhs);
	m_rigidbody = rhs.m_rigidbody;
	return *this;
}

Bullet& Bullet::operator=(Bullet&& rhs) {
	Object::operator=(rhs);
	m_rigidbody = rhs.m_rigidbody;
	return *this;
}

void Bullet::start() {
	timer.reset();
}

void Bullet::draw() {
	Model->drawRaw();		
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
	if (timer.getElapsedTimeSec() >= lifeSpan){
		return true;
	}
	return false;
}

void Bullet::Init(AssimpModel* model) {
	Bullet::Model = model;
}