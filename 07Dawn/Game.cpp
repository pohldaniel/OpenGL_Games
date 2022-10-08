#include "Game.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {
	newZone = ZoneManager::Get().getCurrentZone();
	m_interface = new Interface();

	Mouse::instance().hideCursor(true);

}

Game::~Game() {}

void Game::fixedUpdate() {

}

void Game::update() {
	ViewPort::get().update(m_dt);
}

void Game::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);


	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_BLEND);


	newZone->drawZoneBatched();
	m_interface->DrawCursor(!m_hideInGameCursor);
	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_hideInGameCursor = event.titleBar;
	Mouse::instance().hideCursor(!m_hideInGameCursor);
}