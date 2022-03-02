#include "Game.h"

#include "Random.h"
#include "Pause.h"


//#include "Key Check.hpp"

Game::Game(StateMachine& machine) : State(machine){
	InitAssets();
	InitEntities();
	InitWalls();
	InitSprites();
	InitCountdown();
	InitTimers();

	m_shader = new Shader("shader/quad.vs", "shader/quad.fs");
	m_quad = new Quad();

	m_quadBackground = new Quad(false, 1.0f, 1.2f, 1.0f, 1.0f);
	m_transBackground = Matrix4f::Translate(0.0, -0.2, 0.0);
}

Game::~Game() {
	delete m_player;
}

void Game::fixedUpdate() {
	if (m_player->isAlive())
		FixedUpdateEntities();
}

void Game::update() {
	if (m_player->isAlive()) {
		UpdateEntities();
		//m_bestTime = m_timeClock.getElapsedTime().asSeconds();
	}
	
	//UpdateCountdown();

	if ((Globals::CONTROLLS & Globals::KEY_Q)) {
		i_machine.addStateAtTop(new Pause(i_machine));
	}
}

void Game::render(unsigned int &frameBuffer) {

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", m_transBackground);
	m_quadBackground->render(m_Sprites["background"]);
	glUseProgram(0);
	
	m_player->render();

	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	m_quad->render(m_Sprites["foreground"]);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Game::FixedUpdateEntities() {
	m_player->fixedUpdate();
}

void Game::UpdateEntities() {
	m_player->resolveCollision(m_walls);
	m_player->update();
	UpdateTimers();
}

void Game::InitEntities() {
	m_player = new Player(i_dt, i_fdt);
}

void Game::UpdateTimers() {
	m_enemySpawnTimer.Update(i_dt);
	m_ghostSpawnTimer.Update(i_dt);
	m_gameSpeedTimer.Update(i_dt);
}

void Game::InitTimers() {
	m_enemySpawnTimer.SetFunction(1.0f, [&]() {		
		const float velocity = 450.0f * Random::RandFloat(1.01f, 1.82f);
	});

	m_gameSpeedTimer.SetFunction(5.5f, [&]() {
		const float sub = Random::RandFloat(0.085f, 0.125f);
		const float uTime = m_enemySpawnTimer.GetUpdateTime() - sub;

		m_enemySpawnTimer.SetUpdateTime
		(
			uTime < 0.285f ? 0.285f : uTime
		);

		m_ghostSpawnTimer.SetUpdateTime(m_ghostSpawnTimer.GetUpdateTime() - sub < 7.185f ? 7.185f : m_ghostSpawnTimer.GetUpdateTime() - sub);
	});
}

void Game::UpdateCountdown() {
	//m_timeClock.restart();
}

void Game::InitCountdown() {
	
}

void Game::InitSprites() {
	
	m_Sprites["foreground"] = m_TextureManager.Get("foreground").getTexture();

	/*auto& sf = m_Sprites["foreground"];
	sf = m_TextureManager.Get("foreground").getTexture();
	sf.setTexture(m_TextureManager.Get("foreground"));
	sf.setScale(sf::Vector2f(2.0f, 2.0f));
	sf.setPosition(sf::Vector2f(0.0f, 4.0f));*/
	
	m_Sprites["background"] = m_TextureManager.Get("background").getTexture();
	
	/*auto& sb = m_Sprites["background"];
	sb = m_TextureManager.Get("background").getTexture();

	sb.setTexture(m_TextureManager.Get("background"));
	sb.setScale(sf::Vector2f(6.0f, 6.0f));
	sb.setPosition(sf::Vector2f(0.0f, 4.0f));*/
	m_Sprites["player"] = m_TextureManager.Get("player").getTexture();
}

void Game::InitWalls() {
	m_walls.push_back(Wall(Vector2f(WIDTH / 2.0f, 855.0f), Vector2f(WIDTH, 100.0f)));
	m_walls.push_back(Wall(Vector2f(800.0f, 772.0f), Vector2f(64.0f, 64.0f)));
	m_walls.push_back(Wall(Vector2f(640.0f, 660.0f), Vector2f(128.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(1072.0f, 660.0f), Vector2f(96.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(192.0f, 596.0f), Vector2f(192.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(435.0f, 530.0f), Vector2f(160.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(880.0f, 530.0f), Vector2f(224.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(658.0f, 436.0f), Vector2f(96.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(80.0f, 466.0f), Vector2f(96.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(1182.0f, 436.0f), Vector2f(192.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(398.0f, 340.0f), Vector2f(224.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(815.0f, 340.0f), Vector2f(96.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(1377.0f, 340.0f), Vector2f(128.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(176.0f, 244.0f), Vector2f(160.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(960.0f, 244.0f), Vector2f(192.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(1230.0f, 180.0f), Vector2f(160.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(1490.0f, 116.0f), Vector2f(160.0f, 32.0f)));
}

void Game::InitAssets() {
	m_TextureManager.Load("player", "res/textures/player.png");
	m_TextureManager.Load("background", "res/textures/background.png");
	m_TextureManager.Load("foreground", "res/textures/map.png");		
}
