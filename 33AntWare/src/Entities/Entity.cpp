#include "Entity.h"
#include "Globals.h"

Entity::Entity(std::shared_ptr<aw::Mesh> mesh, aw::Material material) : MeshNode(mesh), m_isStatic(false), material(material){

}

Entity::Entity(AssimpModel* model, aw::Material material) : MeshNode(model), m_isStatic(false), material(material) {

}

Entity::Entity(std::shared_ptr<aw::Mesh> mesh, AssimpModel* model, aw::Material material) : MeshNode(mesh, model), m_isStatic(false), material(material) {

}

void Entity::draw(const Camera& camera) {	
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

void Entity::constructAABB() {

	auto vertices = meshPtr->getVertices();
	aabb.backward = vertices[0].z;
	aabb.forward = vertices[0].z;
	aabb.down = vertices[0].y;
	aabb.up = vertices[0].y;
	aabb.right = vertices[0].x;
	aabb.left = vertices[0].x;

	for (unsigned i = 0; i < vertices.size(); ++i) {
		aabb.backward = glm::min(aabb.backward, vertices[i].z);
		aabb.forward = glm::max(aabb.forward, vertices[i].z);
		aabb.down = glm::min(aabb.down, vertices[i].y);
		aabb.up = glm::max(aabb.up, vertices[i].y);
		aabb.left = glm::min(aabb.left, vertices[i].x);
		aabb.right = glm::max(aabb.right, vertices[i].x);
	}

	for (unsigned i = 0; i < 8; ++i)
		aabb.bounds[i] = { 0, 0, 0 };

	aabb.bounds[0] = { aabb.right, aabb.up, aabb.forward };
	aabb.bounds[1] = { aabb.right, aabb.up, aabb.backward };
	aabb.bounds[2] = { aabb.right, aabb.down, aabb.forward };
	aabb.bounds[3] = { aabb.right, aabb.down, aabb.backward };

	for (unsigned i = 4; i < 8; ++i) {
		aabb.bounds[i] = aabb.bounds[i - 4];
		aabb.bounds[i].x = aabb.left;
	}
}

void Entity::recalculateAABB() {

	glm::mat4 transformationMat;

	transformationMat[0][0] = m_modelMatrix[0][0]; transformationMat[0][1] = m_modelMatrix[0][1]; transformationMat[0][2] = m_modelMatrix[0][2]; transformationMat[0][3] = m_modelMatrix[0][3];
	transformationMat[1][0] = m_modelMatrix[1][0]; transformationMat[1][1] = m_modelMatrix[1][1]; transformationMat[1][2] = m_modelMatrix[1][2]; transformationMat[1][3] = m_modelMatrix[1][3];
	transformationMat[2][0] = m_modelMatrix[2][0]; transformationMat[2][1] = m_modelMatrix[2][1]; transformationMat[2][2] = m_modelMatrix[2][2]; transformationMat[2][3] = m_modelMatrix[2][3];
	transformationMat[3][0] = m_modelMatrix[3][0]; transformationMat[3][1] = m_modelMatrix[3][1]; transformationMat[3][2] = m_modelMatrix[3][2]; transformationMat[3][3] = m_modelMatrix[3][3];

	glm::vec3 transformedBound = transformationMat * glm::vec4(aabb.bounds[0], 1);
	aabb.backward = transformedBound.z;
	aabb.forward = transformedBound.z;
	aabb.down = transformedBound.y;
	aabb.up = transformedBound.y;
	aabb.right = transformedBound.x;
	aabb.left = transformedBound.x;

	for (unsigned i = 0; i < 8; ++i) {
		transformedBound = transformationMat * glm::vec4(aabb.bounds[i], 1);
		aabb.backward = glm::min(aabb.backward, transformedBound.z);
		aabb.forward = glm::max(aabb.forward, transformedBound.z);
		aabb.down = glm::min(aabb.down, transformedBound.y);
		aabb.up = glm::max(aabb.up, transformedBound.y);
		aabb.left = glm::min(aabb.left, transformedBound.x);
		aabb.right = glm::max(aabb.right, transformedBound.x);
	}

}