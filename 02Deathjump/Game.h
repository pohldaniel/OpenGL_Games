#pragma once
#include <sstream>

#include "Constants.h"
#include "AssetManger.h"
#include "Extern.h"
#include "StateMachine.h"
#include "Player.h"
#include "Wall.h"
#include "Timer.h"

//#include "Shader Library.hpp"


class Game : public State {
public:
	Game(StateMachine& machine);
	~Game();

	virtual void FixedUpdate() override;
	virtual void Update() override;
	virtual void Render(unsigned int &m_frameBuffer) override;

private:
	Player* m_player;


	//sf::RectangleShape m_fog;
	//sf::Clock m_clock;

	std::unordered_map<std::string, unsigned int> m_Sprites;

	AssetManager<Texture> m_TextureManager;
	//AssetManager<sf::Font> m_FontManager;

	std::vector<Wall> m_walls;

	//sf::Clock m_spawnClock;
	//sf::Clock m_timeClock;
	//sf::Clock m_heartSpawnClock;
	//sf::Clock m_ghostSpawnClock;

	Quad *m_quad;
	Shader *m_shader;
	Texture *m_texture;
	Texture *m_background;


	Timer m_enemySpawnTimer;
	Timer m_ghostSpawnTimer;
	Timer m_gameSpeedTimer;

	float m_bestTime = 0.0f;

	void SpawnHeart();

	void FixedUpdateEntities();
	void UpdateEntities();
	void InitEntities();

	void UpdateTimers();
	void InitTimers();

	void UpdateCountdown();
	void InitCountdown();

	void InitSprites();

	void InitWalls();

	void InitAssets();
};