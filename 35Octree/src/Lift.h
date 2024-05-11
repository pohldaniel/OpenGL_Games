#pragma once

#include <memory>
#include <Physics/Physics.h>
#include <engine/scene/ShapeNode.h>

class Lift {

public:

	Lift();
	~Lift();

	void fixedUpdate(float fdt);

	void initialize(ShapeNode* shapeNodeLift, btCollisionObject* collisionObjectLift, const Vector3f &finishPosition, ShapeNode* shapeNodeButton, btCollisionObject* collisionObjectButton, btCollisionObject* collisionObjectLiftTrigger);
	void setLiftSpeed(float maxLiftSpeed);
	ShapeNode* getShapeNode();
	btCollisionObject* getCollisionObject();

	void setTransitionCompleted(int toState);
	void buttonPressAnimate(bool pressed);
	void handleButtonStartCollision();
	void handleButtonEndCollision();

protected:

	ShapeNode* m_shapeNodeLift;
	ShapeNode* m_shapeNodeButton;
	btCollisionObject* m_collisionObjectLift;
	btCollisionObject* m_collisionObjectButton;
	btCollisionObject* m_collisionObjectLiftTrigger;

	Vector3f m_initialPosition;
	Vector3f m_finishPosition;
	Vector3f m_directionToFinish;
	float   m_totalDistance;
	float   m_maxLiftSpeed;
	float   m_minLiftSpeed;
	float   m_curLiftSpeed;

	bool    m_buttonPressed;
	float   m_buttonPressedHeight;
	bool    m_standingOnButton;

	// states
	int m_liftButtonState;
	enum LiftButtonStateType{
		LIFT_BUTTON_UP,
		LIFT_BUTTON_POPUP,
		LIFT_BUTTON_DOWN
	};

	int m_liftState;
	enum LiftStateType{
		LIFT_STATE_START,
		LIFT_STATE_MOVETO_FINISH,
		LIFT_STATE_MOVETO_START,
		LIFT_STATE_FINISH
	};
};