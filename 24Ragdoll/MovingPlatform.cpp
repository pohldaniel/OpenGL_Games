#include <iostream>
#include "MovingPlatform.h"

MovingPlatform::MovingPlatform() : maxLiftSpeed_(5.0f), minLiftSpeed_(1.5f),  curLiftSpeed_(0.0f){

}

MovingPlatform::~MovingPlatform(){

}

void MovingPlatform::Initialize(StaticModel* platformNode, btRigidBody* body, const Vector3 &finishPosition, bool updateBodyOnPlatform) {
	// get other lift components
	model_ = platformNode;
	platformBody_ = body;
	// positions
	initialPosition_ = model_->WorldPosition();
	finishPosition_ = finishPosition;
	directionToFinish_ = (finishPosition_ - initialPosition_).Normalized();

	// state
	platformState_ = PLATFORM_STATE_MOVETO_FINISH;
	curLiftSpeed_ = maxLiftSpeed_;
}

StaticModel* MovingPlatform::getModel() {
	return model_;
}

void MovingPlatform::FixedUpdate(float timeStep) {
	Vector3 platformPos = model_->Position();
	Vector3 newPos = platformPos;

	// move platform
	if (platformState_ == PLATFORM_STATE_MOVETO_FINISH)
	{
		Vector3 curDistance = finishPosition_ - platformPos;
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
			curLiftSpeed_ = maxLiftSpeed_;
			platformState_ = PLATFORM_STATE_MOVETO_START;
		}
		model_->SetPosition(newPos);
		platformBody_->getMotionState()->setWorldTransform(Physics::BtTransform(btVector3(newPos.x, newPos.y, newPos.z)));
	}
	else if (platformState_ == PLATFORM_STATE_MOVETO_START)
	{
		Vector3 curDistance = initialPosition_ - platformPos;
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
			curLiftSpeed_ = maxLiftSpeed_;
			platformState_ = PLATFORM_STATE_MOVETO_FINISH;
		}

		model_->SetPosition(newPos);
		platformBody_->getMotionState()->setWorldTransform(Physics::BtTransform(btVector3(newPos.x, newPos.y, newPos.z)));
	}
}
