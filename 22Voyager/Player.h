#pragma once
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <GL/glew.h>

#include "Physics.h"
#include "Weapon.h"
#include "SpotLight.h"
#include "Terrain.h"

class Player {
public:
	~Player();

	static Player& GetInstance() {
		static Player instance;
		return instance;
	}

	Player(Player const&) = delete;
	void operator=(Player const&) = delete;

	void Init();
	void ProcessInput();
	void Update(Terrain& terrain, float dt);
	void Switch();

	void ReduceHealth(float amount)		{ m_health -= amount; }
	void Respawn();
	
	int GetHealth()						{ return m_health; }
	bool IsPlayerJumping()				{ return m_jumping; }
	bool IsPlayerFiring()				{ return m_muzzleFlash; }
	bool IsPlayerAiming()				{ return m_sniperAiming; }
	bool IsUsingSniper()			    { return m_sniperEquipped; }

	bool IsPlayerDead()					{ return m_dead; }
	Weapon& GetCurrWeapon()				{ return *m_currWeapon; }
	SpotLight* GetSpotLight()			{ return m_spotLight;}
	void Animate(float dt);
	const Camera& getCamera() const { return m_camera; }
	

	ShaderVo m_shader;

private:
	Player();


	bool m_move, m_sprinting, m_jumping, m_isInAir, m_firing, m_reloading, m_muzzleFlash, m_usingAR, m_swapping, m_swapped;
	bool m_sniperAiming, m_sniperEquipped, m_toggleFlashlight, m_dead;
	int m_health;
	float m_jumpHeight, m_upwardSpeed;

	glm::vec3 m_pos;
	SpotLight* m_spotLight;
	Weapon* m_currWeapon, *m_assaultRifle, *m_sniperRifle;
	Camera m_camera;

	float dx = 0.0f;
	float dy = 0.0f;
	//bool move = false;
	Vector3f directrion;
};

#endif // !__PLAYER_H__