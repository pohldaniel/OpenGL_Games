#include "VehicleObject.h"
#include "Globals.h"

void VehicleObject::UpdateModelMatrix() {

	btTransform vTrans = vehicle.GetTransform();
	vehiclePosition = vehicle.GetTransform().getOrigin();
	btQuaternion vehicleRotation = vTrans.getRotation();

	glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(vehiclePosition.x(), vehiclePosition.y() - 1.2f, vehiclePosition.z()));

	glm::quat glmVehicleRotation = glm::quat(vehicleRotation.w(), vehicleRotation.x(), vehicleRotation.y(), vehicleRotation.z());

	glm::mat4 rotation = glm::mat4_cast(glmVehicleRotation);
	glm::mat4 rotate90DEG_Adjustment = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	objModelMatrix = translation * rotation * rotate90DEG_Adjustment * glm::scale(glm::vec3(0.7f));


	// Update wheel matrices
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

void VehicleObject::draw() {

	UpdateModelMatrix();

	Globals::textureManager.get("car_albedo").bind(0u);

	shader->loadMatrix("modelMatrix", (const float*)glm::value_ptr(objModelMatrix));
	glBindVertexArray(m_model->vao);
	glDrawElements(GL_TRIANGLES, m_model->GetIndices().size(), GL_UNSIGNED_INT, 0);
	
	for (glm::mat4 wheelMatrix : wheelMatrices) {
	
		shader->loadMatrix("modelMatrix", (const float*)glm::value_ptr(wheelMatrix));
		glBindVertexArray(m_wheel->vao);
		glDrawElements(GL_TRIANGLES, m_wheel->GetIndices().size(), GL_UNSIGNED_INT, 0);
		
	}
	glBindVertexArray(0);
}

const btTransform& VehicleObject::getWorldTransform() const {
	return vehicle.vehicle->getRigidBody()->getWorldTransform();
}

const btVector3& VehicleObject::getLinearVelocity() const {
	return vehicle.vehicle->getRigidBody()->getLinearVelocity();
}

void VehicleObject::roate(float x, float y, float z) {
	btTransform& worldTrans = vehicle.vehicle->getRigidBody()->getWorldTransform();
	worldTrans.setRotation(worldTrans.getRotation() * Physics::QuaternionFrom(Quaternion(x, y, z)));
	worldTrans.setOrigin(worldTrans.getOrigin() + btVector3(0.0f, 2.0f, 0.0f));
	vehicle.vehicle->getRigidBody()->setWorldTransform(worldTrans);
}