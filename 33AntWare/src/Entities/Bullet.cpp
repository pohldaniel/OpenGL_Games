#include "Bullet.h"

std::shared_ptr<aw::Mesh> Bullet::Mesh = nullptr;
aw::Material Bullet::Material;

Bullet::Bullet(Vector3f projectVec) : Entity(Bullet::Mesh, Bullet::Material){

	Vector3f directon = Vector3f::Normalize(projectVec) * speed;
	rigidbody.velocity[0] = directon[0];
	rigidbody.velocity[1] = directon[1];
	rigidbody.velocity[2] = directon[2];
	m_isStatic = false;
} 

Bullet::Bullet(Bullet const& rhs) {
	meshPtr = rhs.meshPtr;
	material = rhs.material;
	m_isStatic = rhs.m_isStatic;

	rigidbody.velocity[0] = rhs.rigidbody.velocity[0];
	rigidbody.velocity[1] = rhs.rigidbody.velocity[1];
	rigidbody.velocity[2] = rhs.rigidbody.velocity[2];
}

//neccesary for std::vector.erase()
Bullet& Bullet::operator=(const Bullet& rhs) {
	meshPtr = rhs.meshPtr;
	material = rhs.material;
	m_isStatic = rhs.m_isStatic;

	rigidbody.velocity[0] = rhs.rigidbody.velocity[0];
	rigidbody.velocity[1] = rhs.rigidbody.velocity[1];
	rigidbody.velocity[2] = rhs.rigidbody.velocity[2];

	return *this;
}

void Bullet::start() {
	timer.reset();
}

void Bullet::update(const float dt) {
}

bool Bullet::timeOut() {
	if (timer.getElapsedTimeSec() >= lifeSpan){
		return true;
	}
	return false;
}

void Bullet::Init(std::shared_ptr<aw::Mesh> mesh, aw::Material material) {
	Bullet::Mesh = mesh;
	Bullet::Material = material;
}