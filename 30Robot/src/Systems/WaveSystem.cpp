#include <Event/EventListener.h>
#include <Services/Locator.h>

#include "WaveSystem.h"

WaveSystem::WaveSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Progression& progression, Level& level)
: ISystem(registry, emitter), m_progression(progression), m_enemyFactory(registry, level), m_waveState(WaveState::NOT_STARTED),
  m_frameCount(0), m_timer(0)
{
	m_emitter.on<evnt::ChangeGameState>([this](const evnt::ChangeGameState& event, EventEmitter& emitter) {
		this->m_waveState = WaveState::NOT_STARTED;
	});

	m_emitter.on<evnt::StartWave>([this](const evnt::StartWave & event, EventEmitter & emitter) {
		this->m_waveState = WaveState::PENDING;
		this->m_probaSpawnKamikaze = (float) this->m_progression.getKamikazeNumber() / (this->m_progression.getRobotNumber() + this->m_progression.getKamikazeNumber());
		//spdlog::info(this->m_probaSpawnKamikaze);
		this->m_timer = 3; // Time for the animation
		//IAudio& audioService = entt::ServiceLocator<IAudio>::ref();
		//audioService.stopAllSounds();
		//audioService.playSound(AudioFiles::MUSIC_WAVE);
	});
}

void WaveSystem::update(float deltatime) {
	switch (m_waveState) {
	case WaveState::NOT_STARTED:
		break;

	case WaveState::PENDING:
		m_frameCount++;
		if (m_frameCount >= FRAMERATE) {
			m_timer--;
			m_frameCount = 0;
			m_emitter.publish<evnt::WaveUpdated>(m_timer, m_waveState);
		}
		if (m_timer <= 0) {
			m_waveState = WaveState::DURING;
			m_timer = (m_progression.getRobotNumber()+m_progression.getKamikazeNumber()) * m_progression.getWaveRate();
			m_rateTimer = 0;
		}
		break;

	case WaveState::DURING:
		m_frameCount++;
		if (m_frameCount >= FRAMERATE) {
			// One second elapsed
			m_frameCount = 0;
			m_timer--;
			m_rateTimer--;
			m_emitter.publish<evnt::WaveUpdated>(m_timer, m_waveState);
			if (m_rateTimer <= 0) {
				m_rateTimer = m_progression.getWaveRate();
				int robotNb = m_progression.getRobotNumber();
				int kamikazeNb = m_progression.getKamikazeNumber();
				if (robotNb > 0 && kamikazeNb > 0) {
					IRandom& random = entt::ServiceLocator<IRandom>::ref();
					if (random.random() < m_probaSpawnKamikaze) {
						m_enemyFactory.createKamikaze();
						m_progression.decreaseKamikazeNumber();
					}
					else {
						m_enemyFactory.createRobot();
						m_progression.decreaseRobotNumber();
					}
				}
				else if (robotNb > 0) {
					m_enemyFactory.createRobot();
					m_progression.decreaseRobotNumber();
				}
				else if (kamikazeNb > 0) {
					m_enemyFactory.createKamikaze();
					m_progression.decreaseKamikazeNumber();
				}
			}
		}
		if (m_timer <= 0) {
			m_waveState = WaveState::DONE;
			m_emitter.publish<evnt::WaveUpdated>(0, m_waveState);
		}
		break;

	case WaveState::DONE:
		break;

	default:
		break;
	}
}
