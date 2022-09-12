#include "Game.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {
	newZone = new Zone();
	newZone->loadZone("res/_lua/zone1");

	m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.orthographic(0.0f, static_cast<float>(WIDTH), 0.0f, static_cast<float>(HEIGHT), -1.0f, 1.0f);

	Batchrenderer::get().setCamera(ViewPort::get().getCamera());
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

	//newZone->drawZoneBatched();
	newZone->drawZoneInstanced();
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
}