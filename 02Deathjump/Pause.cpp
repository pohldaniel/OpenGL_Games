#include "Pause.h"
#include "Game.h"

Pause::Pause(StateMachine& machine) : State(machine) {
	InitAssets();	
	InitSprites();
	m_shader = new Shader("shader/quad.vs", "shader/quad.fs");
	m_quad = new Quad();
}

Pause::~Pause() {}

void Pause::fixedUpdate() {}

void Pause::update() {
	if ((Globals::CONTROLLS & Globals::KEY_Q)) {
		i_isRunning = false;
	}
}

void Pause::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	m_quad->render(m_Sprites["background"]);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Pause::InitSprites() {
	m_Sprites["background"] = m_TextureManager.Get("background").getTexture();
}

void Pause::InitAssets() {
	m_TextureManager.Load("background", "res/textures/menu.png");
}