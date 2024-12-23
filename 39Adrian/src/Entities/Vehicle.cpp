#include "Vehicle.h"

Vehicle::Vehicle(const MeshSequence& meshSequence) : SequenceNode(meshSequence, BoundingBox(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, 1.0f, 1.0f))), Entity() {

}

Vehicle::~Vehicle() {

}

void Vehicle::draw() {

	if (m_materialIndex >= 0)
		Material::GetMaterials()[m_materialIndex].bind();

	if (m_textureIndex >= 0)
		Material::GetTextures()[m_textureIndex].bind();

	updateModelMatrix();
	
	shader->use();
	shader->loadMatrix("modelMatrix", m_objModelMatrix);
	meshSequence.draw(0);

	for (const Matrix4f& wheelMatrix : m_wheelMatrices) {
		shader->loadMatrix("modelMatrix", wheelMatrix);
		meshSequence.draw(1);
	}
}

void Vehicle::updateModelMatrix() {
	const btTransform& vTrans = m_vehicle.getTransform();
	Matrix4f translation = Matrix4f::Translate(Physics::VectorFrom(vTrans.getOrigin() - btVector3(0.0f, 1.2f, 0.0f)));
	Matrix4f rotation = Matrix4f::Rotate(Physics::QuaternionFrom(vTrans.getRotation()));
	Matrix4f rotateAdjustment = Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), -90.0f);
	m_objModelMatrix = translation * rotation * rotateAdjustment * Matrix4f::Scale(0.7f);

	m_wheelMatrices.clear();

	for (int i = 0; i < m_vehicle.getNumWheels(); i++) {
		const btWheelInfo &wheelinfo = m_vehicle.getWheelInfo(i);
		Matrix4f wheelM = Physics::MatrixFrom(wheelinfo.m_worldTransform);
		Matrix4f centeringTranslation = Matrix4f::Translate(0.0f, -0.5f, 0.0f);
		m_wheelMatrices.push_back(wheelM * centeringTranslation * rotateAdjustment * Matrix4f::Scale(0.5f));
	}
}

const Matrix4f Vehicle::getWorldTransform() {
	return Physics::MatrixFrom(m_vehicle.getRigidBody()->getWorldTransform());
}

const Vector3f Vehicle::getLinearVelocity() {
	return Physics::VectorFrom(m_vehicle.getRigidBody()->getLinearVelocity());
}

const Vector3f Vehicle::getPosition(){
	return Physics::VectorFrom(m_vehicle.getRigidBody()->getWorldTransform().getOrigin());
}

void Vehicle::roate(float x, float y, float z) {
	btTransform& worldTrans = m_vehicle.getRigidBody()->getWorldTransform();
	worldTrans.setRotation(worldTrans.getRotation() * Physics::QuaternionFrom(Quaternion(x, y, z)));
	worldTrans.setOrigin(worldTrans.getOrigin() + btVector3(0.0f, 2.0f, 0.0f));
	m_vehicle.getRigidBody()->setWorldTransform(worldTrans);
}

void Vehicle::applyEngineForce(float force) {
	m_vehicle.applyEngineForce(force);
}

void Vehicle::applySteer(float steer) {
	m_vehicle.applySteer(steer);
}