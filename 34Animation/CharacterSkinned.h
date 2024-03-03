#pragma once

#include <engine/Camera.h>
#include <engine/Clock.h>
#include <Animation/AnimationController.h>

class QueInput {
public:
	QueInput() : input_(UINT_MAX), holdTime_(1200) {}

	void SetInput(unsigned input){
		input_ = input;
		queTimer_.reset();
	}

	unsigned GetInput() const{
		return input_;
	}

	void Update(){
		if (queTimer_.getElapsedTimeMilli() >= holdTime_){
			input_ = UINT_MAX;
		}
	}

	bool Empty() const{
		return (input_ == UINT_MAX);
	}

	void Reset(){
		input_ = UINT_MAX;
	}

protected:
	unsigned input_;

	unsigned holdTime_;
	Clock queTimer_;
};

class CharacterSkinned {

	const float INAIR_THRESHOLD_TIME = 0.1f;
	const float MOVE_SPEED = 8.0f;
	const float JUMP_TIMER = 1.0f;

public:

	CharacterSkinned();

	void draw(const Camera& camera);
	void update(const float dt);
	void fixedUpdate(float fdt);
	void processWeaponAction(bool equip, bool lMouseB);

	AnimatedModel m_model;
	AnimationController* m_animationController;

	bool m_onGround;
	bool m_okToJump;
	bool m_jumpStarted;
	float m_inAirTimer;
	float m_jumpTimer;
	Vector3f m_curMoveDir;

	std::string weaponActionAnim_;
	unsigned int weaponActionState_;
	QueInput queInput_;
	unsigned comboAnimsIdx_;
	std::vector<std::string> weaponComboAnim_;

	enum WeaponState { Weapon_Invalid, Weapon_Unequipped, Weapon_Equipping, Weapon_Equipped, Weapon_UnEquipping, Weapon_AttackAnim };
	enum AnimLayerType { NormalLayer, WeaponLayer };
	enum WeaponDmgState { WeaponDmg_OFF, WeaponDmg_ON };

	bool equipWeapon;
	bool lMouseB;

	Bone *m_rightHandLocatorNode, *m_swordLocatorNode, *m_locatorNode, *m_armorLocatorNode;
};