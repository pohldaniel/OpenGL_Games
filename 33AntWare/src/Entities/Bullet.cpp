#include "Bullet.h"

std::shared_ptr<aw::Mesh> Bullet::Mesh = nullptr;
aw::Material Bullet::Material;

Bullet::Bullet(const Vector3f& projectVec) : Object(){

	Vector3f directon = Vector3f::Normalize(projectVec) * speed;
	rigidbody.velocity[0] = directon[0];
	rigidbody.velocity[1] = directon[1];
	rigidbody.velocity[2] = directon[2];
	//m_isStatic = false;
} 

Bullet::Bullet(Bullet const& rhs) {
	transform = rhs.transform;
	rigidbody = rhs.rigidbody;
}

Bullet::Bullet(Bullet&& rhs){
	transform = rhs.transform;
	rigidbody = rhs.rigidbody;
}

//neccesary for std::vector.erase()
Bullet& Bullet::operator=(const Bullet& rhs) {
	transform = rhs.transform;
	rigidbody = rhs.rigidbody;
	return *this;
}

void Bullet::start() {
	timer.reset();
}

void Bullet::draw(const Camera& camera) {
	Material.apply();
	Mesh->draw();
}

void Bullet::update(const float dt) {

	//std::cout << "Pos: " << m_position[0] << "  " << m_position[1] << "  " << m_position[2] << std::endl;
}

void Bullet::fixedUpdate(float fdt) {
	rigidbody.velocity += rigidbody.acceleration * fdt;
	rigidbody.angularVelocity += rigidbody.angularAcceleration * fdt;

	/*Vector3f appliedVelocity = Quaternion::Rotate(m_orientation, Vector3f(rigidbody.velocity.x, rigidbody.velocity.y, rigidbody.velocity.z));
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
	rotate(appliedAngularVelocity * fdt);*/


	glm::vec3 _appliedVelocity = glm::mat3(transform.getRotation()) * rigidbody.velocity;
	glm::vec3 _appliedAngularVelocity = glm::mat3(transform.getRotation()) * rigidbody.angularVelocity;
	if (rigidbody.isLinearLocked(aw::AXIS::x))
		_appliedVelocity.x = 0;
	if (rigidbody.isLinearLocked(aw::AXIS::y))
		_appliedVelocity.y = 0;
	if (rigidbody.isLinearLocked(aw::AXIS::z))
		_appliedVelocity.z = 0;
	if (rigidbody.isAngularLocked(aw::AXIS::x))
		_appliedAngularVelocity.x = 0;
	if (rigidbody.isAngularLocked(aw::AXIS::y))
		_appliedAngularVelocity.y = 0;
	if (rigidbody.isAngularLocked(aw::AXIS::z))
		_appliedAngularVelocity.z = 0;
	transform.translateGlobal(_appliedVelocity * fdt);
	transform.rotateGlobal(_appliedAngularVelocity * fdt);
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

glm::mat4 Bullet::getModelMatrix() {
	glm::vec3 translation = transform.getPosition();
	glm::vec3 scale = transform.getScale();

	glm::mat4 transMat = glm::translate(glm::mat4(1.0f), translation);
	glm::mat4 rotationMat = glm::mat4(transform.getRotation());
	glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);

	glm::mat4 transform = transMat * rotationMat * scaleMat;

	return transform;
}