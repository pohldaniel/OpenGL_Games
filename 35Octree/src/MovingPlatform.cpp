#include "MovingPlatform.h"

MovingPlatform::MovingPlatform() : m_maxLiftSpeed(5.0f), m_minLiftSpeed(1.5f), m_curLiftSpeed(0.0f) {

}

MovingPlatform::~MovingPlatform() {

}

void MovingPlatform::initialize(ShapeNode* platformNode, btCollisionObject* collisionShape, const Vector3f &finishPosition) {
	// get other lift components
	m_shapeNode = platformNode;
	m_platformShape = collisionShape;
	// positions
	m_initialPosition = platformNode->getWorldPosition();
	m_finishPosition = finishPosition;
	m_directionToFinish = Vector3f::Normalize(m_finishPosition - m_initialPosition);

	// state
	m_platformState = PLATFORM_STATE_MOVETO_FINISH;
	m_curLiftSpeed = m_maxLiftSpeed;
}

ShapeNode* MovingPlatform::getShapeNode() {
	return m_shapeNode;
}

void MovingPlatform::setPlatformSpeed(float maxLiftSpeed) { 
	m_maxLiftSpeed = maxLiftSpeed; 
}

void MovingPlatform::fixedUpdate(float fdt) {
	Vector3f platformPos = m_shapeNode->getPosition();
	Vector3f newPos = platformPos;

	// move platform
	if (m_platformState == PLATFORM_STATE_MOVETO_FINISH){
		Vector3f curDistance = m_finishPosition - platformPos;
		float dist = curDistance.length();
		Vector3f curDirection = Vector3f::Normalize(curDistance);			
		float dotd = Vector3f::Dot(m_directionToFinish, curDirection);

		if (dotd > 0.0f){
			// slow down near the end
			if (dist < 1.0f){
				m_curLiftSpeed *= 0.92f;
			}
			m_curLiftSpeed = Math::Clamp(m_curLiftSpeed, m_minLiftSpeed, m_maxLiftSpeed);
			newPos += curDirection * m_curLiftSpeed * fdt;

		}else{
			newPos = m_finishPosition;
			m_curLiftSpeed = m_maxLiftSpeed;
			m_platformState = PLATFORM_STATE_MOVETO_START;
		}		
		m_shapeNode->setPosition(newPos);
		m_platformShape->setWorldTransform(Physics::BtTransform(Physics::VectorFrom(newPos)));

	}else if (m_platformState == PLATFORM_STATE_MOVETO_START){
		Vector3f curDistance = m_initialPosition - platformPos;
		float dist = curDistance.length();
		Vector3f curDirection = Vector3f::Normalize(curDistance);
		float dotd = Vector3f::Dot(m_directionToFinish, curDirection);

		if (dotd < 0.0f){
			// slow down near the end
			if (dist < 1.0f){
				m_curLiftSpeed *= 0.92f;
			}
			m_curLiftSpeed = Math::Clamp(m_curLiftSpeed, m_minLiftSpeed, m_maxLiftSpeed);
			newPos += curDirection * m_curLiftSpeed * fdt;

		}else{
			newPos = m_initialPosition;
			m_curLiftSpeed = m_maxLiftSpeed;
			m_platformState = PLATFORM_STATE_MOVETO_FINISH;
		}

		m_shapeNode->setPosition(newPos);
		m_platformShape->setWorldTransform(Physics::BtTransform(Physics::VectorFrom(newPos)));
	}
}