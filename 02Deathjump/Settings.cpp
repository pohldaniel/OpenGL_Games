#include "Settings.h"

Settings::Settings(StateMachine& machine) : State(machine) {
	initSprites();

	m_text = new Text("SETTINGS", 200.0f / 90.0f);
	m_text->setPosition(Vector2f(WIDTH * 0.5f, 120.0f) - m_text->getSize() * 0.5f);

	m_shader = Globals::shaderManager.getAssetPointer("quad");
	m_shaderBlur = Globals::shaderManager.getAssetPointer("blur");
	m_quad = new Quad(false);

	m_button = Button("BACK", Vector4f(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 80.0f / 255.0f));
	m_button.setPosition(Vector2f(180.0f, HEIGHT - 80.0f));
	m_button.setOrigin(m_button.getSize() * 0.5f);
	m_button.setOutlineThickness(4.0f);

	Transition& transition = Transition::get();

	m_button.setFunction([&]() {
		transition.setFunction([&]() {
			i_isRunning = false;
			transition.start(Mode::Unveil);
		});
		transition.start(Mode::Veil);
	});

	m_emitter = new ParticleEmitter(Vector4f(1.0f, 1.0f, 0.0f, 1.0f), Vector4f(1.0f, 0.0f, 1.0f, 0.0f), 75);
	m_emitter->setLifeTimeRange(3.5f, 8.5f);
	m_emitter->setDirection(Vector2f(1, 0));
	m_emitter->setPosition(Vector2f(725.0f, 750.0f));
	m_emitter->setParticleMax(100);


	m_seekerBar = new SeekerBar(Vector2f(350.0f, HEIGHT - 180.0f), 10, 5);
}

Settings::~Settings() {
	delete m_quad;
	delete m_text;
	delete m_emitter;
	m_button.~Button();	
}

void Settings::fixedUpdate() {}

void Settings::update() {
	m_button.update();
	m_seekerBar->update();

	m_emitter->addParticles();
	m_emitter->update(i_dt);
}

void Settings::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glUseProgram(m_shaderBlur->m_program);
	m_shaderBlur->loadFloat("u_blur_radius", 0.008f);
	m_quad->render(m_sprites["background"]);
	glUseProgram(0);

	//m_text->render(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	m_button.render();
	m_seekerBar->render();

	glEnable(GL_BLEND);
	m_emitter->render();
	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Settings::initSprites() {
	m_sprites["background"] = Globals::textureManager.get("menu").getTexture();
}