#pragma once
#include <sstream>

#include "Constants.h"
#include "AssetManger.h"
#include "StateMachine.h"
#include "Player.h"
#include "Wall.h"
#include "Timer.h"

class Game : public State {
public:
	Game(StateMachine& machine);
	~Game();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &m_frameBuffer) override;

	

private:

	void InitCountdown();
	void InitSprites();
	void InitWalls();
	void InitAssets();

	Player* m_player;

	std::unordered_map<std::string, unsigned int> m_Sprites;

	AssetManager<Texture> m_TextureManager;

	std::vector<Wall> m_walls;

	Quad *m_quad;
	Quad *m_quadBackground;
	Shader *m_shader;
	Texture *m_texture;
	Texture *m_background;

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

	void UpdateCountdown();
	
};