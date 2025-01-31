#pragma once

#include <unordered_map>
#include "engine/input/Keyboard.h"
#include "engine/Camera.h"
#include "Physics.h"
#include "turso3d/Scene/SpatialNode.h"
#include "turso3d/Renderer/AnimationController.h"
#include "Lift.h"
class KinematicCharacterController;

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
struct MovingData
{
	MovingData() : node_(0) {}

	MovingData& operator =(const MovingData& rhs)
	{
		node_ = rhs.node_;
		transform_ = rhs.transform_;
		return *this;
	}
	bool operator == (const MovingData& rhs)
	{
		return (node_ && node_ == rhs.node_);
	}

	SpatialNode *node_;
	Matrix3x4 transform_;
};

struct CharacterTriggerCallback : public btCollisionWorld::ContactResultCallback {
	CharacterTriggerCallback() {}
	btScalar CharacterTriggerCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override;
	SpatialNode* button_;
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
	Character(AnimatedModel* model, AnimationController* animationController, KinematicCharacterController* kcc, Camera& camera, SpatialNode* button, Lift* lift);

	
	/// Handle physics world update. Called by LogicComponent base class.
	void FixedUpdate(float timeStep);
	void FixedPostUpdate(float timeStep);
	void HandleCollision(btCollisionObject* collisionObject);
	void HandleCollisionButton(btCollisionObject* collisionObject);
	void ProcessCollision();
	void BeginCollision();
	void EndCollision();

	void SetOnMovingPlatform(btRigidBody* platformBody){
		//onMovingPlatform_ = (platformBody != NULL);
		//platformBody_ = platformBody; 
	}
	void NodeOnMovingPlatform(SpatialNode *node);

	bool onGround_;
	bool okToJump_;
	float inAirTimer_;

	// extra vars
	Vector3 curMoveDir_;
	bool isJumping_;
	bool jumpStarted_;

	WeakPtr<btCollisionShape> collisionShape_;
	WeakPtr<AnimationController> animController_;
	WeakPtr<KinematicCharacterController> kinematicController_;

	// moving platform data
	MovingData movingData_[2];
	AnimatedModel* model_;
	CharacterTriggerCallback m_characterTriggerResult;
	CharacterTriggerCallbackButton m_characterTriggerResultButton;
	SpatialNode* button_;
	Camera& camera;
	Lift* lift_;
};
