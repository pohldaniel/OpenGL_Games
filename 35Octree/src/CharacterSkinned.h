#pragma once

#include <engine/Camera.h>
#include <engine/Clock.h>
#include <engine/scene/ShapeNode.h>
#include <engine/scene/BoneNode.h>
#include <engine/scene/AnimationNode.h>
#include <engine/Clock.h>
#include <Animation/AnimationController.h>
#include <Physics/Physics.h>


class Lift;
class KinematicCharacterController;
class CharacterSkinned {

	const float MOVE_SPEED = 8.0f;
	const float JUMP_TIMER = 1.0f;

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

	class QueInput {
	public:
		QueInput() : input_(UINT_MAX), holdTime_(1200) {}

		void SetInput(unsigned input) {
			input_ = input;
			queTimer_.reset();
		}

		unsigned GetInput() const {
			return input_;
		}

		void Update() {
			if (queTimer_.getElapsedTimeMilli() >= holdTime_) {
				input_ = UINT_MAX;
			}
		}

		bool Empty() const {
			return (input_ == UINT_MAX);
		}

		void Reset() {
			input_ = UINT_MAX;
		}

	protected:
		unsigned input_;

		unsigned holdTime_;
		Clock queTimer_;
	};

	struct MovingData {
		MovingData() : node_(nullptr) {}

		MovingData& operator =(const MovingData& rhs) {
			node_ = rhs.node_;
			transform_ = rhs.transform_;
			return *this;
		}

		bool operator == (const MovingData& rhs) {
			return (node_ && node_ == rhs.node_);
		}

		SceneNodeLC *node_;
		Matrix4f transform_;
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

	struct CharacterTriggerCallbackWeapon : public btCollisionWorld::ContactResultCallback {
		CharacterTriggerCallbackWeapon(Clock& timer) : damageTimer(timer) {}
		btScalar CharacterTriggerCallbackWeapon::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override;
		Clock& damageTimer;
	};


public:

	CharacterSkinned(KinematicCharacterController* kcc, SceneNodeLC* button, Lift* lift, Camera& camera);
	~CharacterSkinned();

	void draw(const Camera& camera);
	void update(const float dt);
	void fixedUpdate(float fdt);
	void fixedPostUpdate(float fdt);
	void handleCollision(btCollisionObject* collisionObject);
	void handleCollisionButton(btCollisionObject* collisionObject);
	void handleCollisionWeapon(btCollisionObject* collisionObject);
	void processCollision();
	void beginCollision();
	void endCollision();

	void processWeaponAction(bool equip, bool lMouseB);
	void rotate(const float pitch, const float yaw, const float roll);
	const Vector3f& getWorldPosition() const;
	const Vector4f& getDummyColor() const;
	void setPosition(const Vector3f& position);
	void nodeOnMovingPlatform(SceneNodeLC *node);

	AnimatedModel m_model;
	Shape m_swordShape, m_armorShape;

	bool m_onGround;
	bool m_okToJump;
	bool m_jumpStarted;
	float m_inAirTimer;
	bool m_isJumping;
	Vector3f m_curMoveDir;

	std::string weaponActionAnim_;
	unsigned int weaponActionState_;
	QueInput queInput_;
	unsigned comboAnimsIdx_;
	std::vector<std::string> weaponComboAnim_;
	unsigned weaponDmgState_;

	enum WeaponState { Weapon_Invalid, Weapon_Unequipped, Weapon_Equipping, Weapon_Equipped, Weapon_UnEquipping, Weapon_AttackAnim };
	enum AnimLayerType { NormalLayer, WeaponLayer };
	enum WeaponDmgState { WeaponDmg_OFF, WeaponDmg_ON };

	bool equipWeapon;
	bool lMouseB;

	BoneNode *m_rightHandLocatorNode, *m_swordLocatorNode, *m_armorLocatorNode;
	ShapeNode* m_sword;
	AnimationNode* m_animationNode;
	AnimationController* m_animationController;

	MovingData m_movingData[2];
	CharacterTriggerCallback m_characterTriggerResult;
	CharacterTriggerCallbackButton m_characterTriggerResultButton;
	CharacterTriggerCallbackWeapon m_characterTriggerResultWeapon;

	Lift* m_lift;
	KinematicCharacterController* m_kinematicController;
	Camera& camera;
	btRigidBody* m_swordBody;
	Clock m_damageTimer;
};