#include <iostream>
#include <engine/scene/AnimationNode.h>

#include "Character.h"
#include "CharacterController.h"
#include "MovingPlatform.h"


Character::Character(AnimationNode* model, AnimationController* animationController, CharacterController* kcc, Camera& camera, SceneNodeLC* button, Lift* lift)
	: model_(model),
	animController_(animationController),
	kinematicController_(kcc),
	onGround_(false),
	okToJump_(true),
	inAirTimer_(0.0f),
	jumpStarted_(false),
	camera(camera),
	button_(button)
{

	kinematicController_->setUserPointer(this);
	m_characterTriggerResult.button_ = button;
	lift_ = lift;
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
	const Quaternion& rot = model_->getOrientation();
	Vector3f moveDir = Vector3f::ZERO;
	onGround_ = kinematicController_->OnGround();

	if (keyboard.keyDown(Keyboard::KEY_W))
		moveDir += Vector3f::BACK;
	if (keyboard.keyDown(Keyboard::KEY_S))
		moveDir += Vector3f::FORWARD;
	if (keyboard.keyDown(Keyboard::KEY_A))
		moveDir += Vector3f::LEFT;
	if (keyboard.keyDown(Keyboard::KEY_D))
		moveDir += Vector3f::RIGHT;

	// Normalize move vector so that diagonal strafing is not faster
	if (moveDir.lengthSq() > 0.0f)
		Vector3f::Normalize(moveDir);

	// rotate movedir
	curMoveDir_ = Quaternion::Rotate(rot, moveDir);

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
				animController_->PlayExclusive("beta_jump_start", 0, false, 0.2f);
				animController_->SetTime("beta_jump_start", 0);
			}
		}
		else
		{
			okToJump_ = true;
		}
	}

	if (!onGround_ || jumpStarted_){
		if (jumpStarted_){
			if (animController_->IsAtEnd("beta_jump_start")){
				animController_->PlayExclusive("beta_jump_loop", 0, true, 0.3f);
				animController_->SetTime("beta_jump_loop", 0);
				jumpStarted_ = false;
			}
		}else{
			const float maxDistance = 50.0f;
			const float segmentDistance = 10.01f;
			PhysicsRaycastResult result;

			Physics::RaycastSingleSegmented(result, model_->getPosition(), Vector3f::DOWN, maxDistance, segmentDistance, Physics::collisiontypes::RAY, Physics::collisiontypes::FLOOR);

			//float slopeDot = result.normal_.DotProduct(Vector3(0.0f, 1.0f, 0.0f));

			if (result.distance_ > 0.7f) {
				animController_->PlayExclusive("beta_jump_loop", 0, true, 0.2f);
			}
			//else if (result.body_ == NULL)
			//{
			//    // fall to death animation
			//}
		}
	}else{
		// Play walk animation if moving on ground, otherwise fade it out
		if ((softGrounded) && !moveDir.compare(Vector3f::ZERO, EPSILON)){
			animController_->PlayExclusive("beta_run", 0, true, 0.2f);
		}else{
			animController_->PlayExclusive("beta_idle", 0, true, 0.2f);
		}
	}
}

void Character::FixedPostUpdate(float timeStep) {
	if (movingData_[0] == movingData_[1]) {
		Matrix4f delta = movingData_[0].transform_ * movingData_[1].transform_.inverse();
		// add delta
		Vector3f kPos;
		Quaternion kRot;
		kinematicController_->GetTransform(kPos, kRot);

		Matrix4f matKC(kPos, kRot, Vector3f::ONE);	
		matKC = delta * matKC;
		kinematicController_->SetTransform(matKC.getTranslation(), Quaternion(matKC.getRotation()));

		float deltaYaw = Quaternion(delta.getRotation()).getYaw();
		model_->rotate(Quaternion(Vector3f::UP, deltaYaw));
		camera.rotate(- deltaYaw * 1.0f / camera.getRotationSpeed(), 0.0f, kPos);

		Vector3f _pos = kinematicController_->GetPosition();		
	}

	// update node position
	model_->setPosition(kinematicController_->GetPosition());

	// shift and clear
	movingData_[1] = movingData_[0];
	movingData_[0].node_ = 0;
}

void Character::NodeOnMovingPlatform(SceneNodeLC* node) {
	movingData_[0].node_ = node;
	movingData_[0].transform_ = node->getWorldTransformation();

	//std::cout << "Position: " << movingData_[0].transform_[3][0] << "  " << movingData_[0].transform_[3][1] << "  " << movingData_[0].transform_[3][2] << std::endl;
}

void Character::ProcessCollision() {
	m_characterTriggerResultButton.prevCollision.first = m_characterTriggerResultButton.currentCollision.first;
	m_characterTriggerResultButton.prevCollision.second = m_characterTriggerResultButton.currentCollision.second;
}

void Character::HandleCollision(btCollisionObject* collisionObject) {
	Physics::GetDynamicsWorld()->contactPairTest(kinematicController_->pairCachingGhostObject_.get(), collisionObject, m_characterTriggerResult);
}

void Character::HandleCollisionButton(btCollisionObject* collisionObject) {
	m_characterTriggerResultButton.currentCollision = std::make_pair(nullptr, nullptr);
	Physics::GetDynamicsWorld()->contactPairTest(kinematicController_->pairCachingGhostObject_.get(), collisionObject, m_characterTriggerResultButton);
}

void Character::BeginCollision() {
	if (m_characterTriggerResultButton.currentCollision.first && m_characterTriggerResultButton.prevCollision.second == nullptr) {
		Lift* lift = reinterpret_cast<Lift*>(m_characterTriggerResultButton.currentCollision.second->getUserPointer());
		lift->handleButtonStartCollision();
	}
}

void Character::EndCollision() {
	if (m_characterTriggerResultButton.currentCollision.first == nullptr && m_characterTriggerResultButton.prevCollision.second) {
		Lift* lift = reinterpret_cast<Lift*>(m_characterTriggerResultButton.prevCollision.second->getUserPointer());
		lift->handleButtonEndCollision();
	}
}

btScalar CharacterTriggerCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
	Character* character = reinterpret_cast<Character*>(colObj0Wrap->getCollisionObject()->getUserPointer());
	SceneNodeLC* model = reinterpret_cast<SceneNodeLC*>(colObj1Wrap->getCollisionObject()->getUserPointer());
	character->NodeOnMovingPlatform(model);
	return 0;
}

btScalar CharacterTriggerCallbackButton::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
	currentCollision.first = colObj0Wrap->getCollisionObject();
	currentCollision.second = colObj1Wrap->getCollisionObject();
	return 0;
}