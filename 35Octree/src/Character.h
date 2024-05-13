#pragma once

#include <unordered_map>
#include <engine/input/Keyboard.h>
#include <engine/scene/SceneNodeLC.h>
#include <engine/Camera.h>
#include <Physics/Physics.h>
#include <Animation/AnimationController.h>
#include "Lift.h"

class KinematicCharacterController;
class AnimationNode;
//=============================================================================
//=============================================================================
const int CTRL_FORWARD = 1;
const int CTRL_BACK = 2;
const int CTRL_LEFT = 4;
const int CTRL_RIGHT = 8;
const int CTRL_JUMP = 16;

const float MOVE_FORCE = 0.2f;
const float INAIR_MOVE_FORCE = 0.06f;
const float BRAKE_FORCE = 0.2f;
const float JUMP_FORCE = 7.0f;
const float YAW_SENSITIVITY = 0.1f;
const float INAIR_THRESHOLD_TIME = 0.1f;

//=============================================================================
//=============================================================================
struct MovingData{
	MovingData() : node_(nullptr) {}

	MovingData& operator =(const MovingData& rhs){
		node_ = rhs.node_;
		transform_ = rhs.transform_;
		return *this;
	}

	bool operator == (const MovingData& rhs){
		return (node_ && node_ == rhs.node_);
	}

	SceneNodeLC *node_;
	Matrix4f transform_;
};

struct CharacterTriggerCallback : public btCollisionWorld::ContactResultCallback {
	CharacterTriggerCallback() {}
	btScalar CharacterTriggerCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override;
	SceneNodeLC* button_;
};

struct CharacterTriggerCallbackButton : public btCollisionWorld::ContactResultCallback {
	CharacterTriggerCallbackButton() {}
	btScalar CharacterTriggerCallbackButton::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override;

	std::pair<const btCollisionObject*, const btCollisionObject*> currentCollision;
	std::pair<const btCollisionObject*, const btCollisionObject*> prevCollision;
};

class Character {

public:
	/// Construct.
	Character(AnimationNode* model, AnimationController* animationController, KinematicCharacterController* kcc, Camera& camera, SceneNodeLC* button, Lift* lift);


	/// Handle physics world update. Called by LogicComponent base class.
	void FixedUpdate(float timeStep);
	void FixedPostUpdate(float timeStep);
	void HandleCollision(btCollisionObject* collisionObject);
	void HandleCollisionButton(btCollisionObject* collisionObject);
	void ProcessCollision();
	void BeginCollision();
	void EndCollision();

	void SetOnMovingPlatform(btRigidBody* platformBody) {
		//onMovingPlatform_ = (platformBody != NULL);
		//platformBody_ = platformBody; 
	}
	void NodeOnMovingPlatform(SceneNodeLC *node);

	bool onGround_;
	bool okToJump_;
	float inAirTimer_;

	// extra vars
	Vector3f curMoveDir_;
	bool isJumping_;
	bool jumpStarted_;

	AnimationController* animController_;
	KinematicCharacterController* kinematicController_;

	// moving platform data
	MovingData movingData_[2];
	AnimationNode* model_;
	CharacterTriggerCallback m_characterTriggerResult;
	CharacterTriggerCallbackButton m_characterTriggerResultButton;
	SceneNodeLC* button_;
	Lift* lift_;
	Camera& camera;
};
