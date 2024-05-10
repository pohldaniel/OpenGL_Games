#include "SplinePlatform.h"

SplinePlatform::SplinePlatform() : m_rotation(-0.3f) {

}

SplinePlatform::~SplinePlatform() {

}

void SplinePlatform::initialize(SplinePath* splinePath, btCollisionObject* collisionShape){
	m_splinePath = splinePath;
	m_collisionShape = collisionShape;
	m_controlNode = splinePath->GetControlledNode();
}

btCollisionObject* SplinePlatform::getCollisionShape() {
	return m_collisionShape;
}

void SplinePlatform::fixedUpdate(float fdt) {

	if (m_splinePath) {
		m_splinePath->Move(fdt);

		// looped path, reset to continue
		if (m_splinePath->IsFinished()) {
			m_splinePath->Reset();
		}

		// rotate
		if (m_controlNode) {
			Quaternion drot(Vector3f(0.0f, 1.0f, 0.0f), m_rotation);
			Quaternion nrot = m_controlNode->getWorldOrientation();
			//m_controlNode->setWorldRotation(nrot * drot);
			m_controlNode->setOrientation(nrot * drot);

			Vector3f curPos = m_controlNode->getPosition();
			m_collisionShape->setWorldTransform(Physics::BtTransform(Physics::VectorFrom(curPos)));

			//rigidBody_->getMotionState()->setWorldTransform(Physics::BtTransform(btVector3(curPos.x, curPos.y, curPos.z)));
		}
	}
}