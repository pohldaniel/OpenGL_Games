#include <iostream>

#include "Character.h"
#include "KinematicCharacterContoller.h"
#include "turso3d/Renderer/AnimatedModel.h"
#include "MovingPlatform.h"

Character::Character(AnimatedModel* model, AnimationController* animationController, KinematicCharacterController* kcc, Camera& camera)
	: model_(model), 
	animController_(animationController), 
	kinematicController_(kcc),
	onGround_(false),
	okToJump_(true),
	inAirTimer_(0.0f),
	jumpStarted_(false),
	camera(camera)
	{

	kinematicController_->setUserPointer(this);
}


void Character::FixedUpdate(float timeStep) {
	Keyboard &keyboard = Keyboard::instance();
	
	// Update the in air timer. Reset if grounded
	if (!onGround_)
		inAirTimer_ += timeStep;
	else
		inAirTimer_ = 0.0f;
	// When character has been in air less than 1/10 second, it's still interpreted as being on ground
	bool softGrounded = inAirTimer_ < INAIR_THRESHOLD_TIME;

	// Update movement & animation
	const QuaternionTu& rot = model_->Rotation();
	Vector3 moveDir = Vector3::ZERO;
	onGround_ = kinematicController_->OnGround();

	if (keyboard.keyDown(Keyboard::KEY_W))
		moveDir += Vector3::BACK;
	if (keyboard.keyDown(Keyboard::KEY_S))
		moveDir += Vector3::FORWARD;
	if (keyboard.keyDown(Keyboard::KEY_A))
		moveDir += Vector3::LEFT;
	if (keyboard.keyDown(Keyboard::KEY_D))
		moveDir += Vector3::RIGHT;
		
	// Normalize move vector so that diagonal strafing is not faster
	if (moveDir.LengthSquared() > 0.0f)
		moveDir.Normalize();

	// rotate movedir
	curMoveDir_ = rot * moveDir;

	kinematicController_->SetWalkDirection(curMoveDir_ * (softGrounded ? MOVE_FORCE : INAIR_MOVE_FORCE));

	if (softGrounded)
	{
		isJumping_ = false;
		// Jump. Must release jump control between jumps
		if (keyboard.keyDown(Keyboard::KEY_SPACE))
		{
			isJumping_ = true;
			if (okToJump_)
			{
				okToJump_ = false;
				jumpStarted_ = true;
				kinematicController_->Jump();

				animController_->StopLayer(0);
				animController_->PlayExclusive("Beta/Beta_JumpStart.ani", 0, false, 0.2f);
				animController_->SetTime("Beta/Beta_JumpStart.ani", 0);
			}
		}
		else
		{
			okToJump_ = true;
		}
	}

	if (!onGround_ || jumpStarted_)
	{
		if (jumpStarted_)
		{
			if (animController_->IsAtEnd("Beta/Beta_JumpStart.ani"))
			{
				animController_->PlayExclusive("Beta/Beta_JumpLoop1.ani", 0, true, 0.3f);
				animController_->SetTime("Beta/Beta_JumpLoop1.ani", 0);
				jumpStarted_ = false;
			}
		}
		else
		{
			const float maxDistance = 50.0f;
			const float segmentDistance = 10.01f;
			PhysicsRaycastResult result;

			Physics::RaycastSingleSegmented(result, Ray(model_->Position(), Vector3::DOWN), maxDistance, segmentDistance, Physics::collisiontypes::RAY, Physics::collisiontypes::FLOOR);

			//float slopeDot = result.normal_.DotProduct(Vector3(0.0f, 1.0f, 0.0f));

			if (result.body_ && result.distance_ > 0.7f)
			{
				animController_->PlayExclusive("Beta/Beta_JumpLoop1.ani", 0, true, 0.2f);
			}
			//else if (result.body_ == NULL)
			//{
			//    // fall to death animation
			//}
		}
	}
	else
	{
		// Play walk animation if moving on ground, otherwise fade it out
		if ((softGrounded) && !moveDir.Equals(Vector3::ZERO))
		{
			animController_->PlayExclusive("Beta/Beta_Run.ani", 0, true, 0.2f);
		}
		else
		{

			animController_->PlayExclusive("Beta/Beta_Idle.ani", 0, true, 0.2f);
		}
	}
}

void Character::FixedPostUpdate(float timeStep) {

	if (movingData_[0] == movingData_[1]){

		Matrix3x4 delta = movingData_[0].transform_ * movingData_[1].transform_.Inverse();


		// add delta
		Vector3 kPos;
		QuaternionTu kRot;
		kinematicController_->GetTransform(kPos, kRot);



		Matrix3x4 matKC(kPos, kRot, Vector3::ONE);

		// update
		matKC = delta * matKC;
		kinematicController_->SetTransform(matKC.Translation(), matKC.Rotation());

		model_->Rotate(QuaternionTu(delta.Rotation().YawAngle(), Vector3::UP));
		camera.rotate(-delta.Rotation().YawAngle() * 10.0f, 0.0f, Vector3f(kPos.x, kPos.y, kPos.z));

	}

	// update node position
	model_->SetWorldPosition(kinematicController_->GetPosition());

	// shift and clear
	movingData_[1] = movingData_[0];
	movingData_[0].node_ = 0;
}

void Character::NodeOnMovingPlatform(SpatialNode *node) {
	
	movingData_[0].node_ = node;
	movingData_[0].transform_ = node->WorldTransform();
}

void Character::HandleCollision(btCollisionObject* collisionObject) {
	Physics::GetDynamicsWorld()->contactPairTest(kinematicController_.Get()->pairCachingGhostObject_.get(), collisionObject, m_characterTriggerResult);
}


btScalar CharacterTriggerCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
	
	Character* character = reinterpret_cast<Character*>(colObj0Wrap->getCollisionObject()->getUserPointer());
	StaticModel* model = reinterpret_cast<StaticModel*>(colObj1Wrap->getCollisionObject()->getUserPointer());
	character->NodeOnMovingPlatform(model);

	return 0;
}