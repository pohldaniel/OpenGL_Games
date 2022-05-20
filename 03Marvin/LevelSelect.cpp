#include "LevelSelect.h"
#include "Game.h"

LevelSelect::LevelSelect(StateMachine& machine) : State(machine, CurrentState::LEVELSELECT) {
	initSprites();

	m_shader = Globals::shaderManager.getAssetPointer("quad");
	m_quadBackground = new Quad(false, -1.0f, 1.0f, -1.0f, 1.0f);

	Button* button = new Button();
	button->setPosition(Vector2f(200.0f, 325.0f));
	button->setState(Button::State::SELECTED);
	button->setLevel("res/Maps/easypeasy2.json");
	m_buttons.push_back(button);

	button = new Button();
	button->setPosition(Vector2f(200.0f, 300.0f));
	button->setLevel("res/Maps/grasslands.json");
	m_buttons.push_back(button);

	button = new Button();
	button->setPosition(Vector2f(200.0f, 275.0f));
	m_buttons.push_back(button);

	button = new Button();
	button->setPosition(Vector2f(200.0f, 250.0f));
	m_buttons.push_back(button);

	m_blocks = m_buttons.size() - 1;
}

LevelSelect::~LevelSelect() {
	delete m_quadBackground;
}

void LevelSelect::fixedUpdate() {}

void LevelSelect::update() {

	unsigned int oldBlock = m_currentBlock;
	
	if (Globals::CONTROLLS & Globals::KEY_S && !(m_guard & Globals::KEY_S)) {
		
		if (m_currentBlock < m_blocks) {
			m_currentBlock++;
		}

		m_guard |= Globals::KEY_S;
	}else {
		m_guard = !(Globals::CONTROLLS & Globals::KEY_S) ? m_guard & ~Globals::KEY_S : m_guard;
	}

	if (Globals::CONTROLLS & Globals::KEY_W && !(m_guard & Globals::KEY_W)) {
		
		if (m_currentBlock > 0) {
			m_currentBlock--;
		}

		m_guard |= Globals::KEY_W;
	}else {
		m_guard = !(Globals::CONTROLLS & Globals::KEY_W) ? m_guard & ~Globals::KEY_W : m_guard;
	}

	if (oldBlock != m_currentBlock) {
		m_buttons[oldBlock]->setState(Button::State::ACTIVE);
		m_buttons[m_currentBlock]->setState(Button::State::SELECTED);
	}

	if (Globals::CONTROLLS & Globals::KEY_RETURN && !(m_guard & Globals::KEY_RETURN)) {

		//...->loadLevel(m_buttons[m_currentBlock]->getLevel());

		m_isRunning = false;
		m_machine.addStateAtBottom(new Game(m_machine));

		m_guard |= Globals::KEY_RETURN;
	}else {
		m_guard = !(Globals::CONTROLLS & Globals::KEY_RETURN) ? m_guard & ~Globals::KEY_RETURN : m_guard;
	}
}

void LevelSelect::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	m_quadBackground->render(m_sprites["background"]);
	glUseProgram(0);

	//m_button->render();

	for (const auto& button : m_buttons) {
		button->render();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LevelSelect::initSprites() {
	m_sprites["background"] = Globals::textureManager.get("background").getTexture();
}
