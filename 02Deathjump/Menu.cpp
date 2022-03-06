#include "Menu.h"


Menu::Menu(StateMachine& machine) : State(machine) {
	initAssets();
	initSprites();

	m_text = new Text("DEATHJUMP!", 200.0f / 90.0f);
	m_text->setPosition(Vector2f(WIDTH * 0.5f, 120.0f) - m_text->getSize() * 0.5f);

	m_shader = new Shader("shader/quad.vs", "shader/quad.fs");
	m_quad = new Quad();
	
	float thikness = 4.0f;
	m_button1 = new Button("START", 290.0f, 65.0f, Vector4f(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 80.0f / 255.0f));
	m_button1->setPosition(Vector2f(WIDTH, HEIGHT) * 0.5f);
	m_button1->setOrigin(m_button1->getSize() * 0.5f);
	m_button1->setOutlineThickness(thikness);

	Transition& transition = Transition::get();

	m_button1->setFunction([&]() {
		transition.setFunction([&]() {
			i_isRunning = false;
			i_machine.addStateAtBottom(new Game(i_machine));

			transition.start(Mode::Unveil);
		});

		transition.start(Mode::Veil);
	});

	m_button2 = new Button("SETTINGS", 450.0f, 65.0f, Vector4f(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 80.0f / 255.0f));
	m_button2->setPosition(Vector2f(WIDTH * 0.5f, 550));
	m_button2->setOrigin(m_button2->getSize() * 0.5f);
	m_button2->setOutlineThickness(thikness);

	m_button2->setFunction([&]() {
		transition.setFunction([&]() {
			i_machine.addStateAtTop(new Settings(i_machine));
			transition.start(Mode::Unveil);
		});

		transition.start(Mode::Veil);
	});

	m_button3 = new Button("EXIT", 235.0f, 65.0f, Vector4f(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 80.0f / 255.0f));
	m_button3->setPosition(Vector2f(WIDTH * 0.5f, 650));
	m_button3->setOrigin(m_button3->getSize() * 0.5f);
	m_button3->setOutlineThickness(thikness);

	m_button3->setFunction([&]() {
		transition.setFunction([&]() { i_isRunning = false; });
		transition.start(Mode::Veil);
	});
}

Menu::~Menu() {}

void Menu::fixedUpdate() {}

void Menu::update() {

	m_button1->update();
	m_button2->update();
	m_button3->update();	
}

void Menu::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(m_shader->m_program);
	m_shader->loadFloat("u_blur_radius", 0.008f);
	m_quad->render(m_Sprites["background"]);
	glUseProgram(0);

	m_text->render(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	m_button1->render();
	m_button2->render();
	m_button3->render();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Menu::initSprites() {
	m_Sprites["background"] = m_TextureManager.Get("background").getTexture();
}

void Menu::initAssets() {
	m_TextureManager.Load("background", "res/textures/menu.png", true, false);
}