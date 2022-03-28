#include "Menu.h"


Menu::Menu(StateMachine& machine) : State(machine) {
	initSprites();

	m_title = new Text("DEATHJUMP!", 200.0f / 90.0f);
	m_title->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	m_title->setPosition(Vector2f(WIDTH * 0.5f, HEIGHT - 120.0f) - m_title->getSize() * 0.5f);


	m_bestTime = new TextField("BEST TIME: " + Text::floatToString(Globals::bestTime, 2));
	m_bestTime->setPosition(Vector2f(WIDTH / 2.0f, HEIGHT - 325.0f));
	m_bestTime->setOrigin(m_bestTime->getSize() * 0.5f);
	m_bestTime->setFillColor(Vector4f(224.0f / 255.0f, 165.0f / 255.0f, 0.0f, 70.0f / 255.0f));
	m_bestTime->setOutlineColor(Vector4f(224.0f / 255.0f, 165.0f / 255.0f, 0.0f, 1.0f));
	m_bestTime->setOutlineThickness(4.0f);
	m_bestTime->setTextColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	
	/*m_bestTime = new TextField(30, Vector4f(224.0f / 255.0f, 165.0f / 255.0f, 0.0f, 70.0f / 255.0f));
	m_bestTime->setPosition(Vector2f(WIDTH / 2.0f, HEIGHT - 325.0f));
	m_bestTime->setOutlineColor(Vector4f(224.0f / 255.0f, 165.0f / 255.0f, 0.0f, 1.0f));*/

	m_shader = Globals::shaderManager.getAssetPointer("quad");
	m_quad = new Quad(false);

	std::initializer_list<std::pair<const std::string, Button>> init =
	{
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

	Globals::musicManager.get("menu").Play();
	Globals::musicManager.get("menu").SetLooping(true);
}

Menu::~Menu() {
	//delete m_quad;
	//delete m_title;
	//delete m_bestTime;
	//for (auto& b : m_buttons)
		//b.second.~Button();
}

void Menu::fixedUpdate() {}

void Menu::update() {
	for (auto& b : m_buttons) 
		b.second.update();

	/*if ((Globals::CONTROLLS & Globals::KEY_E)) {
		if (m_toggle) {
			Globals::musicManager.get("menu").Play();
		}else {
			Globals::musicManager.get("menu").Stop();
			//Globals::musicManager.get("menu").Pause();
			
		}

		m_toggle = !m_toggle;
	}*/
}

void Menu::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	m_quad->render(m_sprites["background"]);
	glUseProgram(0);

	m_title->render();
	m_bestTime->render();

	for (auto& b : m_buttons)
		b.second.render();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Menu::initSprites() {
	m_sprites["background"] = Globals::textureManager.get("menu").getTexture();
}