#pragma once
#include "engine/Camera.h"
#include "RenderableObject.h"
#include "SpotLight.h"

class Terrain;

class Enemy : public RenderableObject {

public:

	Enemy(const Camera& cam);
	~Enemy();

	void draw(const Camera& camera) override;
	void update(const Terrain& terrain, const Camera& cam, float dt);


	void DrawShockwave();

	void ReduceHealth(int amount);
	
	void SetPos(const Vector3f& pos) { m_pos = pos; }
	Vector3f& GetPos() { return m_pos; }

	float CalcDistance(const Vector3f& enemyPos, const Vector3f& playerPos);
	float CalcDistanceNoHeight(const Vector3f& enemyPos, const Vector3f& playerPos);

	void SetAttackDamage(float attkDmg) { m_attackDamage = attkDmg; }
	float GetAttackDamage() { return m_attackDamage; }

	void SetDeath(bool isDead) { m_dead = isDead; }
	bool GetDeath() { return m_dead; }

	void SetRespawnStatus(bool canRespawn);
	bool GetRespawnStatus() { return m_canRespawn; }
	void setSpotlight(SpotLight* spotlight) { m_spotlight = spotlight; }

private:

	const Camera& m_camera;
	Vector3f m_pos, m_velocity, m_fireDir, m_dronePos, m_oldPlayerPos;
 
	float m_maximumSpeed, m_maximumDroneSpeed;
	float m_attackDamage;
	float m_deltaTime, m_currLifeTimer, m_respawnTimer;
	float m_distance, m_damageTakenDuration, m_evadeDurationCounter, m_shootDuration, m_blastRadius;
	int m_health;
	bool m_dead, m_withinAttackRange, m_takingDamage, m_evade, m_evadeRight, m_droneStatus, m_droneActive, m_fire, m_canRespawn, m_droneSelfDestruct;
	bool m_damageToken;
	

	SpotLight* m_spotlight = nullptr;

	// Private functions
	void Seek(const Camera& target, const float dt);
	void Flee(const Camera& target, const float dt);
	void Fire(const Camera& target, const Terrain& terrain, const float dt);
	void Respawn();

	static RenderableObject Drone;
	static RenderableObject Shockwave;
};