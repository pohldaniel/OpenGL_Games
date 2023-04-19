#pragma once

#include "engine/AssimpModel.h"
#include "engine/Shader.h"
#include "RenderableObject.h"
#include "Animation.h"

class Weapon {

public:

	Weapon();
	~Weapon();

	void draw(Camera& camera, const Matrix4f& model, bool bDrawRelativeToCamera = false);
	void init(GLchar* path, const Shader* shader);
	void configure(int maxAmmo, float fireRate, float reloadTime, int damage);
	void update(float dt);
	void fire(Weapon* weapon, Camera& camera, float dt, bool& firing, bool& reloading);
	void reload(Weapon* weapon, Camera& camera, float dt, bool& reloading);
	void restartWeapon();

	inline void setMaxAmmo(int maxAmmo) { m_maxAmmo = maxAmmo; }
	inline int getMaxAmmoCount() { return m_maxAmmo; }
	inline int getAmmoCount() { return m_ammoCount; }

	inline void setFireRate(float fireRate) { m_fireRate = fireRate; }
	inline float getFireRate() { return m_fireRate; }

	inline void setReloadTime(float reloadTime) { m_maxReloadTimer = reloadTime; }
	inline float getReloadTime() { return m_maxReloadTimer; }

	inline void setDamage(int dmg) { m_damage = dmg; }
	inline int getDamage() { return m_damage; }

	inline Animation& getAnimComponent() { return m_animator; }

	unsigned short m_materialIndex;

	

private:

	float m_currFireRateTime, m_fireRate, m_currReloadTime, m_maxReloadTimer;
	int m_ammoCount, m_maxAmmo, m_damage;
	Animation m_animator;
	RenderableObject m_muzzle;
	AssimpModel m_mesh;
	const Shader* m_shader;
	Transform m_transform;
};