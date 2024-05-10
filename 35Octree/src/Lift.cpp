#include "Lift.h"

Lift::Lift()
	: m_liftState(LIFT_STATE_START),
	m_liftButtonState(LIFT_BUTTON_UP),
	m_maxLiftSpeed(5.0f),
	m_minLiftSpeed(1.5f),
	m_curLiftSpeed(0.0f),
	m_buttonPressed(false),
	m_buttonPressedHeight(0.15f),
	m_standingOnButton(false){

}

Lift::~Lift(){
}

void Lift::initialize(ShapeNode* shapeNodeLift, btCollisionObject* collisionObjectLift, const Vector3f &finishPosition, ShapeNode* shapeNodeButton, btCollisionObject* collisionObjectButton){
	// get other lift components
	m_shapeNodeLift = shapeNodeLift;
	m_shapeNodeButton = shapeNodeButton;
	m_collisionObjectLift = collisionObjectLift;
	m_collisionObjectButton = collisionObjectButton;
	// positions
	m_initialPosition = m_shapeNodeLift->getWorldPosition();
	m_finishPosition = finishPosition;
	m_directionToFinish = Vector3f::Normalize(m_finishPosition - m_initialPosition);
	m_totalDistance = (m_finishPosition - m_initialPosition).length();
}

void Lift::setLiftSpeed(float maxLiftSpeed) {
	m_maxLiftSpeed = maxLiftSpeed;
}

ShapeNode* Lift::getShapeNode() {
	return m_shapeNodeLift;
}

btCollisionObject* Lift::getCollisionObject() {
	return m_collisionObjectLift;
}

void Lift::fixedUpdate(float fdt){
	Vector3f liftPos = m_shapeNodeLift->getPosition();
	Vector3f newPos = liftPos;
	Vector3f oldPos = liftPos;
	// move lift
	if (m_liftState == LIFT_STATE_MOVETO_FINISH){
		Vector3f curDistance = m_finishPosition - liftPos;
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
			setTransitionCompleted(LIFT_STATE_FINISH);
		}
		m_shapeNodeLift->setPosition(newPos);
		m_collisionObjectLift->setWorldTransform(Physics::BtTransform(Physics::VectorFrom(newPos)));

		Vector3f buttonPos = (newPos - oldPos) + m_shapeNodeButton->getPosition();
		m_shapeNodeButton->setPosition(buttonPos);
		m_collisionObjectButton->setWorldTransform(Physics::BtTransform(Physics::VectorFrom(buttonPos)));
	}else if (m_liftState == LIFT_STATE_MOVETO_START){
		Vector3f curDistance = m_initialPosition - liftPos;
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
			setTransitionCompleted(LIFT_STATE_START);
		}

		m_shapeNodeLift->setPosition(newPos);
		m_collisionObjectLift->setWorldTransform(Physics::BtTransform(Physics::VectorFrom(newPos)));

		Vector3f buttonPos = (newPos - oldPos) + m_shapeNodeButton->getPosition();
		m_shapeNodeButton->setPosition(buttonPos);
		m_collisionObjectButton->setWorldTransform(Physics::BtTransform(Physics::VectorFrom(buttonPos)));
	}

	// reenable button
	if (!m_standingOnButton && m_liftButtonState == LIFT_BUTTON_DOWN && (m_liftState == LIFT_STATE_START || m_liftState == LIFT_STATE_FINISH)){
		m_liftButtonState = LIFT_BUTTON_UP;
		buttonPressAnimate(false);
	}


}

void Lift::setTransitionCompleted(int toState){
	m_liftState = toState;

	// adjust button
	if (m_liftButtonState == LIFT_BUTTON_UP){
		buttonPressAnimate(false);
	}
}

void Lift::buttonPressAnimate(bool pressed){
	if (pressed){
		m_shapeNodeButton->setPosition(m_shapeNodeButton->getPosition() + Vector3f(0.0f, -m_buttonPressedHeight, 0.0f));
		m_buttonPressed = true;
	}else{
		m_shapeNodeButton->setPosition(m_shapeNodeButton->getPosition() + Vector3f(0.0f, m_buttonPressedHeight, 0.0f));
		m_buttonPressed = false;
	}
}

void Lift::handleButtonStartCollision(){

	m_standingOnButton = true;

	if (m_liftButtonState == LIFT_BUTTON_UP){
		if (m_liftState == LIFT_STATE_START){
			m_liftState = LIFT_STATE_MOVETO_FINISH;
			m_liftButtonState = LIFT_BUTTON_DOWN;
			m_curLiftSpeed = m_maxLiftSpeed;

			// adjust button
			buttonPressAnimate(true);
		}else if (m_liftState == LIFT_STATE_FINISH){
			m_liftState = LIFT_STATE_MOVETO_START;
			m_liftButtonState = LIFT_BUTTON_DOWN;
			m_curLiftSpeed = m_maxLiftSpeed;

			// adjust button
			buttonPressAnimate(true);
		}

		// play sound and animation
	}
}

void Lift::handleButtonEndCollision(){
	m_standingOnButton = false;

	if (m_liftButtonState == LIFT_BUTTON_DOWN){
		// button animation
		if (m_liftState == LIFT_STATE_START || m_liftState == LIFT_STATE_FINISH){
			m_liftButtonState = LIFT_BUTTON_UP;
			buttonPressAnimate(false);
		}
	}
}