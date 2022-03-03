#include "Pause.h"
#include "Game.h"

Pause::Pause(StateMachine& machine) : State(machine) {
	InitAssets();	
	InitSprites();

	m_text = new Text("PAUSED", 200.0f / 90.0f);
	m_text->setPosition(Vector2f(WIDTH * 0.5f, 120.0f) - m_text->getSize() * 0.5f);

	m_shader = new Shader("shader/quad.vs", "shader/quad.fs");
	m_shaderBlur = new Shader("shader/blur.vs", "shader/blur.fs");
	m_quad = new Quad();

	float thikness = 4.0f;
	m_button1 = new Button("RESUME", 364.0f, 65.0f, Vector4f(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 80.0f / 255.0f));
	m_button1->setPosition(Vector2f(WIDTH * 0.5f, 400) - (Vector2f(364.0f, 65.0f) * 0.5f) + Vector2f(thikness, thikness));
	m_button1->setOutlineThickness(thikness);


	m_button2 = new Button("SETTINGS", 450.0f, 65.0f, Vector4f(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 80.0f / 255.0f));
	m_button2->setPosition(Vector2f(WIDTH * 0.5f, 500) - (Vector2f(450.0f, 65.0f) * 0.5f) + Vector2f(thikness, thikness));
	m_button2->setOutlineThickness(thikness);

	m_button3 = new Button("EXIT TO MENU", 633.0f, 65.0f, Vector4f(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 80.0f / 255.0f));
	m_button3->setPosition(Vector2f(WIDTH * 0.5f, 600) - (Vector2f(633.0f, 65.0f) * 0.5f) + Vector2f(thikness, thikness));
	m_button3->setOutlineThickness(thikness);
}

Pause::~Pause() {}

void Pause::fixedUpdate() {}

void Pause::update() {

	m_button1->update();
	m_button2->update();
	m_button3->update();

	if ((Globals::CONTROLLS & Globals::KEY_Q || m_button1->pressed())) {
		i_isRunning = false;
	}
}

void Pause::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(m_shaderBlur->m_program);
	m_shaderBlur->loadMatrix("u_transform", Matrix4f::IDENTITY);
	m_shaderBlur->loadFloat("u_blur_radius", 0.008f);
	m_quad->render(m_Sprites["background"]);
	glUseProgram(0);
	
	m_text->render(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	m_button1->render();
	m_button2->render();
	m_button3->render();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Pause::InitSprites() {
	m_Sprites["background"] = m_TextureManager.Get("background").getTexture();
}

void Pause::InitAssets() {
	m_TextureManager.Load("background", "res/textures/menu.png", true, false);
}