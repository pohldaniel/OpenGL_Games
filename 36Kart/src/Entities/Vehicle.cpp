#include "Vehicle.h"
#include "Globals.h"

Vehicle::Vehicle(const MeshSequence& meshSequence) : SequenceNode(meshSequence, BoundingBox(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, 1.0f, 1.0f))), m_materialIndex(-1), m_textureIndex(-1) {

}

Vehicle::~Vehicle() {

}

void Vehicle::draw() {

	if (m_materialIndex >= 0)
		Material::GetMaterials()[m_materialIndex].bind();

	if (m_textureIndex >= 0)
		Material::GetTextures()[m_textureIndex].bind();

	updateModelMatrix();
	
	auto shader = Globals::shaderManager.getAssetPointer("main");
	shader->loadMatrix("modelMatrix", objModelMatrix);

	Globals::textureManager.get("car_albedo").bind(0u);

	meshSequence.draw(0);
	for (const Matrix4f& wheelMatrix : wheelMatrices) {
		shader->loadMatrix("modelMatrix", wheelMatrix);
		meshSequence.draw(1);
	}
}

void Vehicle::update(const float dt) {

}

const Material& Vehicle::getMaterial() const {
	return Material::GetMaterials()[m_materialIndex];
}

short Vehicle::getMaterialIndex() const {
	return m_materialIndex;
}

void Vehicle::setMaterialIndex(short index) const {
	m_materialIndex = index;
}

short Vehicle::getTextureIndex() const {
	return m_textureIndex;
}

void Vehicle::setTextureIndex(short index) const {
	m_textureIndex = index;
}

void Vehicle::updateModelMatrix() {
	const btTransform& vTrans = vehicle.GetTransform();
	Matrix4f translation = Matrix4f::Translate(Physics::VectorFrom(vTrans.getOrigin() - btVector3(0.0f, 1.2f, 0.0f)));
	Matrix4f rotation = Matrix4f::Rotate(Physics::QuaternionFrom(vTrans.getRotation()));
	Matrix4f rotateAdjustment = Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), -90.0f);
	objModelMatrix = translation * rotation * rotateAdjustment * Matrix4f::Scale(0.7f);

	wheelMatrices.clear();

	for (int i = 0; i < vehicle.vehicle->getNumWheels(); i++) {
		btWheelInfo &wheelinfo = vehicle.vehicle->getWheelInfo(i);
		Matrix4f wheelM = Physics::MatrixFrom(wheelinfo.m_worldTransform);
		Matrix4f centeringTranslation = Matrix4f::Translate(0.0f, -0.5f, 0.0f);
		wheelMatrices.push_back(wheelM * centeringTranslation * rotateAdjustment * Matrix4f::Scale(0.5f));
	}
}

const btTransform& Vehicle::getWorldTransform() const {
	return vehicle.vehicle->getRigidBody()->getWorldTransform();
}

const btVector3& Vehicle::getLinearVelocity() const {
	return vehicle.vehicle->getRigidBody()->getLinearVelocity();
}

void Vehicle::roate(float x, float y, float z) {
	btTransform& worldTrans = vehicle.vehicle->getRigidBody()->getWorldTransform();
	worldTrans.setRotation(worldTrans.getRotation() * Physics::QuaternionFrom(Quaternion(x, y, z)));
	worldTrans.setOrigin(worldTrans.getOrigin() + btVector3(0.0f, 2.0f, 0.0f));
	vehicle.vehicle->getRigidBody()->setWorldTransform(worldTrans);
}