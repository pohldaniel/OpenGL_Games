#include "Bullet.h"

AssimpModel* Bullet::Model = nullptr;

Bullet::Bullet(const Vector3f& projectVec) : Object(){
	Vector3f directon = Vector3f::Normalize(projectVec) * speed;
	rigidbody.velocity[0] = directon[0];
	rigidbody.velocity[1] = directon[1];
	rigidbody.velocity[2] = directon[2];
} 

Bullet::Bullet(Bullet const& rhs) : Object(rhs){
	rigidbody = rhs.rigidbody;
}

Bullet::Bullet(Bullet&& rhs) : Object(rhs) {
	rigidbody = rhs.rigidbody;
}

Bullet& Bullet::operator=(const Bullet& rhs) {
	Object::operator=(rhs);
	rigidbody = rhs.rigidbody;
	return *this;
}

void Bullet::start() {
	timer.reset();
}

void Bullet::draw(const Camera& camera) {
	Model->drawRaw();		
}

void Bullet::update(const float dt) {

}

void Bullet::fixedUpdate(float fdt) {
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

bool Bullet::timeOut() {
	if (timer.getElapsedTimeSec() >= lifeSpan){
		return true;
	}
	return false;
}

void Bullet::Init(AssimpModel* model) {
	Bullet::Model = model;
}