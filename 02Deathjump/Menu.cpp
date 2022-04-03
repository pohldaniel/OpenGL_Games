#include "Menu.h"


Menu::Menu(StateMachine& machine) : State(machine), m_text(Text(Globals::fontManager.get("font_200"))) {
	initSprites();
	initText();
	initButtons();
	initTextField();
	initTimer();

	m_shader = Globals::shaderManager.getAssetPointer("quad");
	m_quad = new Quad(false);

	Globals::musicManager.get("menu").Play();
	Globals::musicManager.get("menu").SetLooping(true);
}

Menu::~Menu() {
	delete m_quad;
}

void Menu::fixedUpdate() {}

void Menu::update() {
	for (auto& b : m_buttons) 
		b.second.update();

	animateText();
}

void Menu::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	m_quad->render(m_sprites["background"]);
	glUseProgram(0);
	glEnable(GL_BLEND);
	m_text.render();
	m_bestTime.render();
	for (auto& b : m_buttons)
		b.second.render();
	glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Menu::initSprites() {
	m_sprites["background"] = Globals::textureManager.get("menu").getTexture();
}

void  Menu::initText() {
	m_text.setLabel("DEATHJUMP!");
	m_text.setPosition(Vector2f(WIDTH * 0.5f, HEIGHT - 120.0f));	
	m_text.setOrigin(m_text.getSize() * 0.5f);
}

void Menu::initButtons() {
	std::initializer_list<std::pair<const std::string, Button>> init = {
		{ "start",	  Button("START"   , Vector2f(WIDTH * 0.5f, HEIGHT - HEIGHT * 0.5f), Vector4f(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 80.0f / 255.0f)) },
		{ "settings", Button("SETTINGS", Vector2f(WIDTH * 0.5f, HEIGHT - 550)   , Vector4f(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 80.0f / 255.0f)) },
		{ "exit",	  Button("EXIT"    , Vector2f(WIDTH * 0.5f, HEIGHT - 650)   , Vector4f(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 80.0f / 255.0f)) },
	};

	m_buttons = init;

	Transition& transition = Transition::get();

	m_buttons["start"].setFunction([&]() {
		transition.setFunction([&]() {
			i_isRunning = false;
			i_machine.addStateAtBottom(new Game(i_machine));
			Globals::musicManager.get("menu").Stop();
			transition.start(Mode::Unveil);
		});
		transition.start(Mode::Veil);
	});

	m_buttons["settings"].setFunction([&]() {
		transition.setFunction([&]() {
			i_machine.addStateAtTop(new Settings(i_machine));
			transition.start(Mode::Unveil);
		});
		transition.start(Mode::Veil);
	});


	m_buttons["exit"].setFunction([&]() {
		transition.setFunction([&]() { i_isRunning = false; });
		transition.start(Mode::Veil);
	});
}

void Menu::initTextField() {
	m_bestTime = TextField("BEST TIME: " + Text::floatToString(Globals::bestTime, 2));
	m_bestTime.setPosition(Vector2f(WIDTH / 2.0f, HEIGHT - 325.0f));
	m_bestTime.setOrigin(m_bestTime.getSize() * 0.5f);
	m_bestTime.setFillColor(Vector4f(224.0f / 255.0f, 165.0f / 255.0f, 0.0f, 70.0f / 255.0f));
	m_bestTime.setOutlineColor(Vector4f(224.0f / 255.0f, 165.0f / 255.0f, 0.0f, 1.0f));
	m_bestTime.setOutlineThickness(4.0f);
	m_bestTime.setTextColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
}

void Menu::initTimer() {
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

void Menu::animateText() {
	m_textAnimTimer.Update(i_dt);
}