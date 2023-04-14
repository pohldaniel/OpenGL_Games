#pragma once

#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/transform.hpp>
#include "engine/AssimpModel.h"
#include "RenderableObject.h"
#include "Animation.h"

class Weapon {

public:

	Weapon();
	~Weapon();

	void draw(Camera& camera, const Matrix4f& model, bool bDrawRelativeToCamera = false);
	void init(GLchar* path);
	void Configure(int maxAmmo, float fireRate, float reloadTime, int damage);
	void Update(float dt);
	void Fire(Weapon* weapon, Camera& camera, float dt, bool& firing, bool& reloading);
	void Reload(Weapon* weapon, Camera& camera, float dt, bool& reloading);
	void RestartWeapon();

	inline void SetMaxAmmo(int maxAmmo) { m_maxAmmo = maxAmmo; }
	inline int GetMaxAmmoCount() { return m_maxAmmo; }
	inline int GetAmmoCount() { return m_ammoCount; }

	inline void SetFireRate(float fireRate) { m_fireRate = fireRate; }
	inline float GetFireRate() { return m_fireRate; }

	inline void SetReloadTime(float reloadTime) { m_maxReloadTimer = reloadTime; }
	inline float GetReloadTime() { return m_maxReloadTimer; }

	inline void SetDamage(int dmg) { m_damage = dmg; }
	inline int GetDamage() { return m_damage; }

	inline Animation& GetAnimComponent() { return m_animator; }

	unsigned short m_materialIndex;

	AssimpModel m_mesh;
	RenderableObject* m_muzzle;
private:

	float m_currFireRateTime, m_fireRate, m_currReloadTime, m_maxReloadTimer;
	int m_ammoCount, m_maxAmmo, m_damage;
	Animation m_animator;
	Transform m_transform;
};