#pragma once

#include <engine/animationNew/AnimationController.h>
#include <engine/scene/ShapeNode.h>
#include <engine/scene/BoneNode.h>
#include <engine/scene/AnimationNode.h>
#include <engine/Clock.h>
#include <engine/Camera.h>

#include <Physics/Physics.h>

class Lift;
class KinematicCharacterController;
class CharacterSkinned {

	const float MOVE_SPEED = 8.0f;
	const float JUMP_TIMER = 1.0f;

	const float MOVE_FORCE = 0.2f;
	const float INAIR_MOVE_FORCE = 0.06f;
	const float BRAKE_FORCE = 0.2f;
	const float JUMP_FORCE = 7.0f;
	const float YAW_SENSITIVITY = 0.1f;
	const float INAIR_THRESHOLD_TIME = 0.1f;

	class QueInput {
	public:
		QueInput() : m_input(UINT_MAX), m_holdTime(1200) {}

		void setInput(unsigned input) {
			m_input = input;
			m_queTimer.reset();
		}

		unsigned getInput() const {
			return m_input;
		}

		void update() {
			if (m_queTimer.getElapsedTimeMilli() >= m_holdTime) {
				m_input = UINT_MAX;
			}
		}

		bool empty() const {
			return (m_input == UINT_MAX);
		}

		void reset() {
			m_input = UINT_MAX;
		}

	protected:
		unsigned m_input;

		unsigned m_holdTime;
		Clock m_queTimer;
	};

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

		SceneNodeLC *m_node;
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

	struct CharacterTriggerCallbackWeapon : public btCollisionWorld::ContactResultCallback {
		CharacterTriggerCallbackWeapon(Clock& timer) : damageTimer(timer) {}
		btScalar CharacterTriggerCallbackWeapon::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override;
		Clock& damageTimer;
	};


public:

	CharacterSkinned(const AnimatedModel& ainamtedModel, Lift* lift, Camera& camera);
	~CharacterSkinned();

	void draw(const Camera& camera);
	void drawOverview(const Matrix4f perspective, const Matrix4f view);
	void update(const float dt);
	void fixedUpdate(float fdt);
	void fixedPostUpdate(float fdt);
	void handleCollision(btCollisionObject* collisionObject);
	void handleCollisionButton(btCollisionObject* collisionObject);
	void handleCollisionWeapon(btCollisionObject* collisionObject);
	void beginCollision();
	void endCollision();
	void processCollision();

	void rotate(const float pitch, const float yaw, const float roll);

	void setPosition(const Vector3f& position);
	const Vector3f& getWorldPosition() const;
	const btVector3 getBtPosition() const;
	const Vector4f getDummyColor() const;

private:

	enum WeaponState { Weapon_Invalid, Weapon_Unequipped, Weapon_Equipping, Weapon_Equipped, Weapon_UnEquipping, Weapon_AttackAnim };
	enum AnimLayerType { NormalLayer, WeaponLayer };
	enum WeaponDmgState { WeaponDmg_OFF, WeaponDmg_ON };

	void processWeaponAction(bool equip, bool lMouseB);	
	void nodeOnMovingPlatform(SceneNodeLC *node);

	Shape m_swordShape, m_armorShape;

	bool m_onGround;
	bool m_okToJump;
	bool m_jumpStarted;
	float m_inAirTimer;
	bool m_isJumping;
	Vector3f m_curMoveDir;

	std::string m_weaponActionAnim;
	unsigned int m_weaponActionState;
	QueInput m_queInput;
	unsigned m_comboAnimsIdx;
	std::vector<std::string> m_weaponComboAnim;
	unsigned m_weaponDmgState;

	bool m_equipWeapon;
	bool m_lMouseB;

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