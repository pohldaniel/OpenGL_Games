#pragma once
#include <sstream>
#include <iomanip>
#include "engine/AssetManger.h"
#include "engine/Clock.h"
#include "engine/Timer.h"
#include "engine/Random.h"

#include "Constants.h"
#include "StateMachine.h"
#include "Player.h"
#include "Wall.h"
#include "Light.h"
#include "Button.h"
#include "ParticleEmitter.h"
#include "Ghost.h"
#include "Fireball.h"
#include "Countdown.h"
#include "Heart.h"

class Game : public State {
public:
	Game(StateMachine& machine);
	~Game();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &m_frameBuffer) override;

private:

	void initSprites();
	void initWalls();
	void initLights();

	Player* m_player;
	Heart* m_heart = nullptr;

	std::unordered_map<std::string, unsigned int> m_sprites;

	std::vector<Wall> m_walls;
	std::vector<Light> m_lights;
	std::vector<Ghost*> m_ghosts;
	std::vector<Fireball*> m_fireballs;

	Shader *m_shader;
	Shader *m_shaderFog;
	Quad *m_fog;
	Quad *m_quad;
	Quad *m_quadBackground;
	
	Text* m_text;
	std::string m_timer;

	Clock m_clock;
	Clock m_timeClock;
	Clock m_heartSpawnClock;

	Timer m_fireballSpawnTimer;
	Timer m_ghostSpawnTimer;
	Timer m_gameSpeedTimer;

	float m_bestTime = 0.0f;
	Countdown* m_countdown;

	Matrix4f m_transBackground = Matrix4f::IDENTITY;

	void updateTimers();
	void initTimers();
	
	void initEntities();	
	void initCountdown();
	void updateCountdown();	
	void initText();

	void updateText();
	void fixedUpdateEntities();
	void updateEntities();

	void spawnHeart();
};