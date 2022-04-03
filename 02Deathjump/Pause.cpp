#include "Pause.h"
#include "Menu.h"

Pause::Pause(StateMachine& machine) : State(machine), m_text(Text(Globals::fontManager.get("font_200"))) {
	initSprites();
	initText();
	initButtons();
	initTimer();

	m_shader = Globals::shaderManager.getAssetPointer("quad");
	m_shaderBlur = Globals::shaderManager.getAssetPointer("blur");
	m_quad = new Quad(false);
	
	Globals::musicManager.get("pause").Play();
	Globals::musicManager.get("pause").SetLooping(true);
}

Pause::~Pause() {
	delete m_quad;
}

void Pause::fixedUpdate() {}

void Pause::update() {
	for (auto& b : m_buttons)
		b.second.update();

	animateText();
}

void Pause::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glUseProgram(m_shaderBlur->m_program);
	m_shaderBlur->loadFloat("u_blur_radius", 0.008f);
	m_quad->render(m_sprites["background"]);
	glUseProgram(0);
	
	glEnable(GL_BLEND);
	m_text.render();

	for (auto& b : m_buttons)
		b.second.render();
	glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Pause::initSprites() {
	m_sprites["background"] = Globals::textureManager.get("menu").getTexture();
}

void Pause::initText() {
	m_text.setLabel("PAUSED");
	m_text.setPosition(Vector2f(WIDTH * 0.5f, HEIGHT - 120.0f));
	m_text.setOrigin(m_text.getSize() * 0.5f);
}

void Pause::initButtons() {
	std::initializer_list<std::pair<const std::string, Button>> init =
	{
		{ "resume",		  Button("RESUME"      , Vector2f(WIDTH * 0.5f, HEIGHT - 400), Vector4f(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 80.0f / 255.0f)) },
		{ "settings",	  Button("SETTINGS"    , Vector2f(WIDTH * 0.5f, HEIGHT - 500), Vector4f(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 80.0f / 255.0f)) },
		{ "exit_to_menu", Button("EXIT TO MENU", Vector2f(WIDTH * 0.5f, HEIGHT - 600), Vector4f(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 80.0f / 255.0f)) },
	};

	m_buttons = init;

	Transition& transition = Transition::get();

	m_buttons["resume"].setFunction([&]() {
		i_isRunning = false;
		Globals::musicManager.get("pause").Stop();
		Globals::musicManager.get("main").Play();
		Globals::musicManager.get("main").SetLooping(true);
	});

	m_buttons["settings"].setFunction([&]() {
		transition.setFunction([&]() {
			i_machine.addStateAtTop(new Settings(i_machine));
			transition.start(Mode::Unveil);
		});
		transition.start(Mode::Veil);
	});

	m_buttons["exit_to_menu"].setFunction([&]() {
		transition.setFunction([&]() {
			Globals::musicManager.get("pause").Stop();
			i_machine.clearAndPush(new Menu(i_machine));
			transition.start(Mode::Unveil);
		});
		transition.start(Mode::Veil);
	});
}

void Pause::initTimer() {
	m_textAnimTimer.SetFunction(0.20f, [&]() {
		constexpr float pos[10] =
		{
			100.0f,
			110.0f,
			120.0f,
			130.0f,
			140.0f,
			130.0f,
			120.0f,
			110.0f,
			100.0f,
			90.0f,
		};
		m_text.setPosition(Vector2f(WIDTH / 2.0f, HEIGHT - pos[m_iterator]));

		m_iterator++;
		if (m_iterator > 9)
			m_iterator = 0;
	});
}

void Pause::animateText() {
	m_textAnimTimer.Update(i_dt);
}