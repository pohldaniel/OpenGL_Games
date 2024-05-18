#pragma once

#include <unordered_map>
#include <engine/input/Keyboard.h>
#include <engine/scene/SceneNodeLC.h>
#include <engine/Camera.h>
#include <Physics/Physics.h>
#include <Animation/AnimationController.h>

class Lift;
class KinematicCharacterController;
class AnimationNode;

class Character {

	const float MOVE_FORCE = 0.2f;
	const float INAIR_MOVE_FORCE = 0.06f;
	const float BRAKE_FORCE = 0.2f;
	const float JUMP_FORCE = 7.0f;
	const float YAW_SENSITIVITY = 0.1f;
	const float INAIR_THRESHOLD_TIME = 0.1f;

	struct MovingData {
		MovingData() : m_node(nullptr) {}

		MovingData& operator =(const MovingData& rhs) {
			m_node = rhs.m_node;
			m_transform = rhs.m_transform;
			return *this;
		}

		bool operator == (const MovingData& rhs) {
			return (m_node && m_node == rhs.m_node);
		}

		SceneNodeLC* m_node;
		Matrix4f m_transform;
	};

	struct CharacterTriggerCallback : public btCollisionWorld::ContactResultCallback {
		CharacterTriggerCallback() {}
		btScalar CharacterTriggerCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override;
	};

	struct CharacterTriggerCallbackButton : public btCollisionWorld::ContactResultCallback {
		CharacterTriggerCallbackButton() {}
		btScalar CharacterTriggerCallbackButton::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override;

		std::pair<const btCollisionObject*, const btCollisionObject*> currentCollision;
		std::pair<const btCollisionObject*, const btCollisionObject*> prevCollision;
	};

public:

	Character(const AnimatedModel& ainamtedModel, Lift* lift, Camera& camera);
	~Character();

	void draw(const Camera& camera);
	void drawOverview(const Matrix4f perspective, const Matrix4f view);
	void update(const float dt);
	void fixedUpdate(float fdt);
	void fixedPostUpdate(float fdt);
	void handleCollision(btCollisionObject* collisionObject);
	void handleCollisionButton(btCollisionObject* collisionObject);
	void processCollision();
	void beginCollision();
	void endCollision();

	void rotate(const float pitch, const float yaw, const float roll);

	const Vector3f& getWorldPosition() const;
	const btVector3 getBtPosition() const;

private:

	void nodeOnMovingPlatform(SceneNodeLC *node);

	bool m_onGround;
	bool m_okToJump;
	float m_inAirTimer;
	Vector3f m_curMoveDir;
	bool m_isJumping;
	bool m_jumpStarted;

	AnimationController* m_animationController;
	KinematicCharacterController* m_kinematicController;

	MovingData m_movingData[2];
	AnimationNode* m_animationNode;
	CharacterTriggerCallback m_characterTriggerResult;
	CharacterTriggerCallbackButton m_characterTriggerResultButton;
	Lift* m_lift;
	Camera& camera;
};
