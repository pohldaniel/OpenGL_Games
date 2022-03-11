#pragma once
#include <sstream>
#include "Clock.h"

#include "Constants.h"
#include "AssetManger.h"
#include "StateMachine.h"
#include "Player.h"
#include "Wall.h"
#include "Timer.h"
#include "Light.h"
#include "Button.h"

#include "ParticleEmitter.h"

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

	std::unordered_map<std::string, unsigned int> m_sprites;

	std::vector<Wall> m_walls;
	std::vector<Light> m_lights;

	Shader *m_shader;
	Shader *m_shaderFog;
	Quad *m_fog;
	Quad *m_quad;
	Quad *m_quadBackground;
	
	Clock m_clock;

	Timer m_enemySpawnTimer;
	Timer m_ghostSpawnTimer;
	Timer m_gameSpeedTimer;

	float m_bestTime = 0.0f;

	Matrix4f m_transBackground = Matrix4f::IDENTITY;


	void FixedUpdateEntities();
	void UpdateEntities();
	void InitEntities();

	void UpdateTimers();
	void InitTimers();
	void InitCountdown();

	void UpdateCountdown();	
};