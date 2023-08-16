#include "Lift.h"

Lift::Lift()
	: liftState_(LIFT_STATE_START),
	liftButtonState_(LIFT_BUTTON_UP),
	maxLiftSpeed_(5.0f),
	minLiftSpeed_(1.5f),
	curLiftSpeed_(0.0f),
	buttonPressed_(false),
	buttonPressedHeight_(15.0f),
	standingOnButton_(false)
{

}

Lift::~Lift()
{
}

void Lift::Initialize(StaticModel* model, btRigidBody* rigidBody, const Vector3 &finishPosition, SpatialNode* liftButton)
{
	// get other lift components
	model_ = model;
	liftButtonNode_ = liftButton;
	rigidBody_ = rigidBody;
	
	// positions
	initialPosition_ = model_->WorldPosition();
	finishPosition_ = finishPosition;
	directionToFinish_ = (finishPosition_ - initialPosition_).Normalized();
	totalDistance_ = (finishPosition_ - initialPosition_).Length();

	// events
	//SubscribeToEvent(liftButtonNode_, E_NODECOLLISIONSTART, URHO3D_HANDLER(Lift, HandleButtonStartCollision));
	//SubscribeToEvent(liftButtonNode_, E_NODECOLLISIONEND, URHO3D_HANDLER(Lift, HandleButtonEndCollision));
}

StaticModel* Lift::getModel() {
	return model_;
}

void Lift::FixedUpdate(float timeStep)
{
	Vector3 liftPos = model_->Position();
	Vector3 newPos = liftPos;

	// move lift
	if (liftState_ == LIFT_STATE_MOVETO_FINISH)
	{
		Vector3 curDistance = finishPosition_ - liftPos;
		Vector3 curDirection = curDistance.Normalized();
		float dist = curDistance.Length();
		float dotd = directionToFinish_.DotProduct(curDirection);

		if (dotd > 0.0f)
		{
			// slow down near the end
			if (dist < 1.0f)
			{
				curLiftSpeed_ *= 0.92f;
			}
			curLiftSpeed_ = Clamp(curLiftSpeed_, minLiftSpeed_, maxLiftSpeed_);
			newPos += curDirection * curLiftSpeed_ * timeStep;
		}
		else
		{
			newPos = finishPosition_;
			SetTransitionCompleted(LIFT_STATE_FINISH);
		}
		model_->SetPosition(newPos);
	}
	else if (liftState_ == LIFT_STATE_MOVETO_START)
	{
		Vector3 curDistance = initialPosition_ - liftPos;
		Vector3 curDirection = curDistance.Normalized();
		float dist = curDistance.Length();
		float dotd = directionToFinish_.DotProduct(curDirection);

		if (dotd < 0.0f)
		{
			// slow down near the end
			if (dist < 1.0f)
			{
				curLiftSpeed_ *= 0.92f;
			}
			curLiftSpeed_ = Clamp(curLiftSpeed_, minLiftSpeed_, maxLiftSpeed_);
			newPos += curDirection * curLiftSpeed_ * timeStep;
		}
		else
		{
			newPos = initialPosition_;
			SetTransitionCompleted(LIFT_STATE_START);
		}
		model_->SetPosition(newPos);
	}

	// reenable button
	if (!standingOnButton_ &&
		liftButtonState_ == LIFT_BUTTON_DOWN &&
		(liftState_ == LIFT_STATE_START || liftState_ == LIFT_STATE_FINISH))
	{
		liftButtonState_ = LIFT_BUTTON_UP;
		ButtonPressAnimate(false);
	}
}

void Lift::SetTransitionCompleted(int toState)
{
	liftState_ = toState;

	// adjust button
	if (liftButtonState_ == LIFT_BUTTON_UP)
	{
		ButtonPressAnimate(false);
	}
}

void Lift::ButtonPressAnimate(bool pressed)
{
	if (pressed)
	{
		liftButtonNode_->SetPosition(liftButtonNode_->Position() + Vector3(0, -buttonPressedHeight_, 0));
		buttonPressed_ = true;
	}
	else
	{
		liftButtonNode_->SetPosition(liftButtonNode_->Position() + Vector3(0, buttonPressedHeight_, 0));
		buttonPressed_ = false;
	}
}