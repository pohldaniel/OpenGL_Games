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
	shader->loadMatrix("modelMatrix", (const float*)glm::value_ptr(objModelMatrix));

	Globals::textureManager.get("car_albedo").bind(0u);

	meshSequence.draw(0);
	for (glm::mat4 wheelMatrix : wheelMatrices) {
		shader->loadMatrix("modelMatrix", (const float*)glm::value_ptr(wheelMatrix));
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

	btTransform vTrans = vehicle.GetTransform();
	btVector3 vehiclePosition = vehicle.GetTransform().getOrigin();
	btQuaternion vehicleRotation = vTrans.getRotation();

	glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(vehiclePosition.x(), vehiclePosition.y() - 1.2f, vehiclePosition.z()));

	glm::quat glmVehicleRotation = glm::quat(vehicleRotation.w(), vehicleRotation.x(), vehicleRotation.y(), vehicleRotation.z());

	glm::mat4 rotation = glm::mat4_cast(glmVehicleRotation);
	glm::mat4 rotate90DEG_Adjustment = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	objModelMatrix = translation * rotation * rotate90DEG_Adjustment * glm::scale(glm::vec3(0.7f));

	wheelMatrices.clear();

	for (int i = 0; i < vehicle.vehicle->getNumWheels(); i++) {
		btWheelInfo &wheelinfo = vehicle.vehicle->getWheelInfo(i);

		glm::mat4 wheelM{ 1.0f };
		wheelinfo.m_worldTransform.getOpenGLMatrix(glm::value_ptr(wheelM));

		glm::vec3 wheelCenterOffset(0.0f, -0.5f, 0.0f); // Adjust Y offset based on your model specifics
		glm::mat4 centeringTranslation = glm::translate(glm::mat4(1.0f), wheelCenterOffset);

		// Optionally apply rotation adjustments if necessary
		glm::mat4 rotateAdjustment = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		wheelMatrices.push_back(
			wheelM * centeringTranslation * rotateAdjustment * glm::scale(glm::vec3(0.5f)) // Apply scaling last to maintain proportions
		);
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