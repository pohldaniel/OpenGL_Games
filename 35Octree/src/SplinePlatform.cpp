#include "SplinePlatform.h"

SplinePlatform::SplinePlatform() : m_rotation(-0.3f) {

}

SplinePlatform::~SplinePlatform() {

}

void SplinePlatform::initialize(SplinePath* splinePath, btCollisionObject* collisionObject){
	m_splinePath = splinePath;
	m_collisionObject = collisionObject;
	m_controlNode = splinePath->GetControlledNode();
}

btCollisionObject* SplinePlatform::getCollisionObject() {
	return m_collisionObject;
}

void SplinePlatform::fixedUpdate(float fdt) {

	if (m_splinePath) {
		m_splinePath->Move(fdt);

		if (m_splinePath->IsFinished()) {
			m_splinePath->Reset();
		}

		// rotate
		if (m_controlNode) {
			Quaternion drot(Vector3f(0.0f, 1.0f, 0.0f), m_rotation);
			Quaternion nrot = m_controlNode->getWorldOrientation() * drot;
			m_controlNode->setOrientation(nrot);

			Vector3f curPos = m_controlNode->getWorldPosition();
			m_collisionObject->setWorldTransform(Physics::BtTransform(Physics::VectorFrom(curPos), Physics::QuaternionFrom(nrot)));
		}
	}
}