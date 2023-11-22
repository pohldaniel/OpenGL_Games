#include "Menu.h"
#include "Game.h"

Menu::Menu(StateMachine& machine) : State(machine, CurrentState::MENU) {
	initSprites();

	m_shader = Globals::shaderManager.getAssetPointer("quad");
	m_quad = new Quad(false, -1.0f, 1.0f, -1.0f, 1.0f);	
}

Menu::~Menu() {
	delete m_quad;
}

void Menu::fixedUpdate() {}

void Menu::update() {

	if (Globals::CONTROLLS & Globals::KEY_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Game(m_machine));
	}
}

void Menu::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	m_quad->render(m_sprites["background"]);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Menu::initSprites() {
	m_sprites["background"] = Globals::textureManager.get("background").getTexture();
}
