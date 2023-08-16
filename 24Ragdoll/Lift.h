#pragma once

#include <memory>
#include "Physics.h"
#include "turso3d/Scene/SpatialNode.h"
#include "turso3d/Renderer/StaticModel.h"

class Lift {

public:

	Lift();
	~Lift();

	void FixedUpdate(float timeStep);

	void Initialize(StaticModel* model, btRigidBody* rigidBody, const Vector3 &finishPosition, SpatialNode* liftButton);
	void SetLiftSpeed(float speed) { maxLiftSpeed_ = speed; }
	StaticModel* getModel();

protected:

	void SetTransitionCompleted(int toState);
	void ButtonPressAnimate(bool pressed);
	//void HandleButtonStartCollision(StringHash eventType, VariantMap& eventData);
	//void HandleButtonEndCollision(StringHash eventType, VariantMap& eventData);

	WeakPtr<StaticModel> model_;
	WeakPtr<SpatialNode> liftButtonNode_;
	btRigidBody* rigidBody_;

	Vector3 initialPosition_;
	Vector3 finishPosition_;
	Vector3 directionToFinish_;
	float   totalDistance_;
	float   maxLiftSpeed_;
	float   minLiftSpeed_;
	float   curLiftSpeed_;

	bool    buttonPressed_;
	float   buttonPressedHeight_;
	bool    standingOnButton_;

	// states
	int liftButtonState_;
	enum LiftButtonStateType
	{
		LIFT_BUTTON_UP,
		LIFT_BUTTON_POPUP,
		LIFT_BUTTON_DOWN
	};

	int liftState_;
	enum LiftStateType
	{
		LIFT_STATE_START,
		LIFT_STATE_MOVETO_FINISH,
		LIFT_STATE_MOVETO_START,
		LIFT_STATE_FINISH
	};
};