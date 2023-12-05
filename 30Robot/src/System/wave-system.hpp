#pragma once

#include <entt/entt.hpp>

#include "ISystem.h"
#include "Event/EventEmitter.h"
#include "Event/start-wave.hpp"
#include "Level/level.hpp"
#include "Entity/enemy-factory.hpp"
#include "progression.hpp"

enum class WaveState {
	NOT_STARTED,
	PENDING,
	DURING,
	DONE
};

class WaveSystem : public ISystem {
public:
	WaveSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Progression& progression, Level& level);
	void update(float deltatime) override;

private:
	Progression & m_progression;
	EnemyFactory m_enemyFactory;
	WaveState m_waveState;
	unsigned int m_frameCount;
	int m_timer;
	int m_rateTimer;
	float m_probaSpawnKamikaze;
};
