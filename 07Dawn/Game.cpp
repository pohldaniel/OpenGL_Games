#include "Game.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {
	newZone = new Zone();
	newZone->loadZone("res/_lua/zone1");

	m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.orthographic(0.0f, static_cast<float>(WIDTH), 0.0f, static_cast<float>(HEIGHT), -1.0f, 1.0f);

	Batchrenderer::get().setCamera(ViewPort::get().getCamera());

	Frames::initFrameTextures();
	mainMenuFrame.reset(new ConfigurableFrame(100, 100, 0, 0, DawnState::MainMenu));

	ConfiguredFrames::fillMainMenuFrame(mainMenuFrame.get());
	mainMenuFrame->setVisible(true);
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
	//newZone->drawZoneInstanced();

	glEnable(GL_DEPTH_TEST);
	mainMenuFrame->draw(0, 0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
}