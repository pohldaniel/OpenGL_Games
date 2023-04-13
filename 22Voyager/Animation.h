#pragma once

#include "engine/Camera.h"
#include "engine/AssimpModel.h"
#include "Model.h"

class Weapon;
class Animation {
public:
	Animation();
	~Animation();

	void PlaySprintFPS(Weapon* weapon, Camera& camera, float dt);
	void PlayWalkFPS(Weapon* weapon, Camera& camera, float dt);
	void PlayIdleFPS(Weapon* weapon, Camera& camera, float dt);
	void PlayFireFPS(Weapon* weapon, Camera& camera, float dt);
	void PlayReloadFPS(Weapon* weapon, Camera& camera, float dt);
	void PlaySwapTwoWeapons(Weapon* weaponA, Weapon* weaponB, Camera& camera, float dt, bool& swapped);
	void PlayFreezeFPS(Weapon* weapon, Camera& camera, float dt);

	void SetWeaponZOffset(float zOffset);
	void SetWeaponYOffset(float yOffset) { m_defWeaponYOffset = yOffset; }
	void SetSprintWeaponZOffset(float zOffset) { m_sprintWeaponZOffset = zOffset; }

private:
	float m_defWeaponRotation;
	float m_defWeaponZOffset;
	float m_sprintWeaponZOffset;
	float m_originalWeaponZOffset;
	float m_defWeaponYOffset;
	bool m_weaponMoveUp;
	bool m_weaponMoveForward;
	bool m_swapping;
	float m_idleYOffset;
};
