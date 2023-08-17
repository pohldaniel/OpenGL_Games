#include <iostream>
#include "Lift.h"

Lift::Lift()
	: liftState_(LIFT_STATE_START),
	liftButtonState_(LIFT_BUTTON_UP),
	maxLiftSpeed_(5.0f),
	minLiftSpeed_(1.5f),
	curLiftSpeed_(0.0f),
	buttonPressed_(false),
	buttonPressedHeight_(0.15f),
	standingOnButton_(false)
{

}

Lift::~Lift()
{
}

void Lift::Initialize(StaticModel* model, btRigidBody* rigidBody, const Vector3 &finishPosition, StaticModel* liftButton, btCollisionObject* collisionObject)
{
	// get other lift components
	model_ = model;
	liftButtonNode_ = liftButton;
	rigidBody_ = rigidBody;
	collisionObjectButton_ = collisionObject;
	// positions
	initialPosition_ = model_->WorldPosition();
	finishPosition_ = finishPosition;
	directionToFinish_ = (finishPosition_ - initialPosition_).Normalized();
	totalDistance_ = (finishPosition_ - initialPosition_).Length();
}

StaticModel* Lift::getModel() {
	return model_;
}

btRigidBody* Lift::getRigidBody() {
	return rigidBody_;
}

void Lift::FixedUpdate(float timeStep)
{
	Vector3 liftPos = model_->Position();
	Vector3 newPos = liftPos;
	Vector3 oldPos = liftPos;
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
		rigidBody_->getMotionState()->setWorldTransform(Physics::BtTransform(btVector3(newPos.x, newPos.y, newPos.z)));
		
		Vector3 buttonPos = (newPos - oldPos) + liftButtonNode_->Position();;
		liftButtonNode_->SetPosition(buttonPos);
		collisionObjectButton_->setWorldTransform(Physics::BtTransform(btVector3(buttonPos.x, buttonPos.y, buttonPos.z)));
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
		rigidBody_->getMotionState()->setWorldTransform(Physics::BtTransform(btVector3(newPos.x, newPos.y, newPos.z)));

		Vector3 buttonPos = (newPos - oldPos) + liftButtonNode_->Position();;
		liftButtonNode_->SetPosition(buttonPos);
		collisionObjectButton_->setWorldTransform(Physics::BtTransform(btVector3(buttonPos.x, buttonPos.y, buttonPos.z)));
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

void Lift::HandleButtonStartCollision()
{
	
	standingOnButton_ = true;

	if (liftButtonState_ == LIFT_BUTTON_UP)
	{
		if (liftState_ == LIFT_STATE_START)
		{
			liftState_ = LIFT_STATE_MOVETO_FINISH;
			liftButtonState_ = LIFT_BUTTON_DOWN;
			curLiftSpeed_ = maxLiftSpeed_;

			// adjust button
			ButtonPressAnimate(true);

			//SetUpdateEventMask(USE_FIXEDUPDATE);
		}
		else if (liftState_ == LIFT_STATE_FINISH)
		{
			liftState_ = LIFT_STATE_MOVETO_START;
			liftButtonState_ = LIFT_BUTTON_DOWN;
			curLiftSpeed_ = maxLiftSpeed_;

			// adjust button
			ButtonPressAnimate(true);

			//SetUpdateEventMask(USE_FIXEDUPDATE);
		}

		// play sound and animation
	}
}

void Lift::HandleButtonEndCollision()
{
	standingOnButton_ = false;

	if (liftButtonState_ == LIFT_BUTTON_DOWN)
	{
		// button animation
		if (liftState_ == LIFT_STATE_START || liftState_ == LIFT_STATE_FINISH)
		{
			liftButtonState_ = LIFT_BUTTON_UP;
			ButtonPressAnimate(false);
		}
	}
}