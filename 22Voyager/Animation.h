#pragma once

#include "engine/Camera.h"
#include "Model.h"

class Animation {
public:
	Animation();
	~Animation();

	void PlaySprintFPS(Model& weapon, Camera& camera, float dt);
	void PlayWalkFPS(Model& weapon, Camera& camera, float dt);
	void PlayIdleFPS(Model& weapon, Camera& camera, float dt);
	void PlayFireFPS(Model& weapon, Camera& camera, float dt);
	void PlayReloadFPS(Model& weapon, Camera& camera, float dt);
	void PlaySwapTwoWeapons(Model& weaponA, Model& weaponB, Camera& camera, float dt, bool& swapped);
	void PlayFreezeFPS(Model& weapon, Camera& camera, float dt);

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
