#pragma once
#include "Model.h"
#include "CameraVo.h"

class Animation {
public:
	Animation();
	~Animation();

	void PlaySprintFPS(Model& weapon, CameraVo& cam, float dt);
	void PlayWalkFPS(Model& weapon, CameraVo& cam, float dt);
	void PlayIdleFPS(Model& weapon, CameraVo& cam, float dt);
	void PlayFireFPS(Model& weapon, CameraVo& cam, float dt);
	void PlayReloadFPS(Model& weapon, CameraVo& cam, float dt);
	void PlaySwapTwoWeapons(Model& weaponA, Model& weaponB, CameraVo& cam, float dt, bool& swapped);
	void PlayFreezeFPS(Model& weapon, CameraVo& cam, float dt);

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
