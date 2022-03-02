#include "Pause.h"
#include "Game.h"

Pause::Pause(StateMachine& machine) : State(machine) {
	InitAssets();	
	InitSprites();
	m_shader = new Shader("shader/quad.vs", "shader/quad.fs");
	m_quad = new Quad();

	m_button = new Button(290.0f, 65.0f, Vector4f(	100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 80.0f / 255.0f));
	m_button->setPosition(Vector2f(WIDTH / 2.0f, 400));
	m_button->setOrigin(Vector2f(0.0f, 65.0f) * 0.5f);
	m_button->setOutlineThickness(4.0f);

	m_text = new Text();
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	m_quad->render(m_Sprites["background"]);
	glUseProgram(0);
	m_button->render();
	m_text->renderText("This is sample text", 25.0f, 25.0f, 1.0f, Vector3f(0.5, 0.8f, 0.2f));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Pause::InitSprites() {
	m_Sprites["background"] = m_TextureManager.Get("background").getTexture();
}

void Pause::InitAssets() {
	m_TextureManager.Load("background", "res/textures/menu.png");
}