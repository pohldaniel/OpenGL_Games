#pragma once

#include "ISystem.h"
#include "Entity/projectile-factory.hpp"
#include "Entity/vfx-factory.hpp"
#include "Components/transform.hpp"

class AttackSystem : public ISystem {
public:
	AttackSystem(entt::DefaultRegistry& registry, EventEmitter& emitter);
	void update(float deltatime) override;

private:
	
	void shootLaser(glm::vec2 pos, float agl, int nbBounce, unsigned int launcherId, float deltatime, bool isTransparent, glm::vec3 col, float launcherAlpha);
	void trySpawnLaserParticle(glm::vec2 pos, float deltatime);
	void glowOnMirror(glm::vec2 pos);
	bool isInRange(cmpt::Transform transform1, float radius1, cmpt::Transform transform2, float radius2);
	bool isInRange(cmpt::Transform transform1, float radius1, unsigned int targetId);
	VFXFactory m_vfxFactory;

	ProjectileFactory m_projectileFactory;
};