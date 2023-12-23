#pragma once

#include <entt/entt.hpp>
#include <Systems/ISystem.h>
#include <EntityFactories/EnemyFactory.h>
#include <Event/EventEmitter.h>
#include <Event/EventListener.h>

#include "Progression.h"

class WaveSystem : public ISystem {
public:
	WaveSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Progression& progression, Level& level);
	void update(float deltatime) override;

private:
	Progression& m_progression;
	EnemyFactory m_enemyFactory;
	WaveState m_waveState;
	unsigned int m_frameCount;
	int m_timer;
	int m_rateTimer;
	float m_probaSpawnKamikaze;
};
