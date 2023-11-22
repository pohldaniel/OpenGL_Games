#include "Game.h"
#include "LevelSelect.h"
Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	m_level = new Level(m_dt, m_fdt);

	m_player = new Player(m_dt, m_fdt);
	m_player->setPosition(m_level->m_playerPosition);

	Globals::world->SetContactListener(new CollisionHandler(*Globals::world, *m_level));
	Globals::world->SetContactFilter(new ContactFilter());
}

Game::~Game() {
	//delete m_player;
	//delete m_level;
}

void Game::fixedUpdate() {
	m_level->fixedUpdate();
	Globals::world->Step(m_fdt, 6, 2);	
	m_player->fixedUpdate();
}

void Game::update() {
	m_player->update();
	m_level->update();

	if (Globals::CONTROLLS & Globals::KEY_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new LevelSelect(m_machine));
	}
}

void Game::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	m_level->render();
	m_player->render();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
