#include "Settings.h"

Settings::Settings(StateMachine& machine) : State(machine, CurrentState::SETTINGS) {
	initSprites();
	initTexts();
	initSeekerBars();
	initButton();
	initTimer();
	initTextFields();

	m_shader = Globals::shaderManager.getAssetPointer("quad");
	m_shaderBlur = Globals::shaderManager.getAssetPointer("blur");
	m_quad = new Quad(false);
}

Settings::~Settings() {
	delete m_quad;
}

void Settings::fixedUpdate() {}

void Settings::update() {
	m_button.update();
	for (auto& s : m_seekerBars)
		s.second.update();
	animateText();
}

void Settings::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glUseProgram(m_shaderBlur->m_program);
	m_shaderBlur->loadFloat("u_blur_radius", 0.008f);
	m_quad->render(m_sprites["background"]);
	glUseProgram(0);

	glEnable(GL_BLEND);
	m_button.render();
	for (auto& t : m_texts) 
		t.second.render();

	for (auto& s : m_seekerBars)
		s.second.render();

	for (auto& tf : m_textFields)
		tf.second.render();

	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Settings::initSprites() {
	m_sprites["background"] = Globals::textureManager.get("menu").getTexture();
}

void Settings::initTexts() {			
	m_texts.insert(std::pair<std::string, Text>("title", Text("SETTINGS", Globals::fontManager.get("font_200"))));	
	m_texts.insert(std::pair<std::string, Text>("sound", Text("SOUND VOLUME", Globals::fontManager.get("font_200"), 90.0f / 200.0f)));
	m_texts.insert(std::pair<std::string, Text>("music", Text("MUSIC VOLUME", Globals::fontManager.get("font_200"), 90.0f / 200.0f)));
	m_texts.insert(std::pair<std::string, Text>("controls", Text("CONTROLS", Globals::fontManager.get("font_200"), 90.0f / 200.0f)));

	m_texts.at("title").setPosition(Vector2f(WIDTH * 0.5f, HEIGHT - 120.0f) - m_texts.at("title").getSize() * 0.5f);
	m_texts.at("title").setOrigin(m_texts.at("title").getSize() * 0.5f);
	m_texts.at("sound").setPosition(Vector2f(WIDTH * 0.5f, HEIGHT - 260.0f) - m_texts.at("sound").getSize() * 0.5f);
	m_texts.at("music").setPosition(Vector2f(WIDTH * 0.5f, HEIGHT - 460.0f) - m_texts.at("music").getSize() * 0.5f);
	m_texts.at("controls").setPosition(Vector2f(WIDTH * 0.5f, HEIGHT - 650.0f) - m_texts.at("music").getSize() * 0.5f);
}

void Settings::initSeekerBars() {
	
	std::initializer_list<std::pair<const std::string, SeekerBar>> init ={
		{ "sound", SeekerBar(Vector2f(350.0f, HEIGHT - 340.0f), 10, Globals::soundVolume * 10) },
		{ "music", SeekerBar(Vector2f(350.0f, HEIGHT - 540.0f), 10, Globals::musicVolume * 10) },
	};

	m_seekerBars = init;

	auto& soundVolume = Globals::soundVolume;
	
	m_seekerBars["sound"].setLeftFunction([&]() {
		if (soundVolume > 0.0f) {
			soundVolume -= 0.1f;
			soundVolume = roundf(soundVolume * 10) / 10;
			m_seekerBars["sound"].m_buttonLeft->m_effectsPlayer.setVolume(soundVolume);
		}
	});

	m_seekerBars["sound"].setRightFunction([&]() {
		if (soundVolume < 1.0f) {
			soundVolume += 0.1f;
			soundVolume = roundf(soundVolume * 10) / 10;
			m_seekerBars["sound"].m_buttonRight->m_effectsPlayer.setVolume(soundVolume);
		}
	});

	auto& musicVolume = Globals::musicVolume;

	m_seekerBars["music"].setLeftFunction([&musicVolume]() {
		if (musicVolume > 0.0f) {
			musicVolume -= 0.1f;
			musicVolume = roundf(musicVolume * 10) / 10;	
		}
	});

	m_seekerBars["music"].setRightFunction([&musicVolume]() {
		if (musicVolume < 1.0f) {
			musicVolume += 0.1f;
			musicVolume = roundf(musicVolume * 10) / 10;
		}
	});
}

void Settings::initTextFields() {

	std::initializer_list<std::pair<const std::string, TextField>> init ={
		{ "w", TextField("W") },
		{ "a", TextField("A") },
		{ "s", TextField("S") },
		{ "d", TextField("D") }
	};

	m_textFields = init;

	m_textFields["w"].setPosition(Vector2f(WIDTH * 0.5 - 260.0f, 150.0f));
	m_textFields["w"].setOrigin(m_textFields["w"].getSize() * 0.5f);
	m_textFields["w"].setFillColor(Vector4f(224.0f / 255.0f, 165.0f / 255.0f, 0.0f, 70.0f / 255.0f));
	m_textFields["w"].setOutlineColor(Vector4f(224.0f / 255.0f, 165.0f / 255.0f, 0.0f, 1.0f));
	m_textFields["w"].setOutlineThickness(4.0f);
	m_textFields["w"].setTextColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

	m_textFields["a"].setPosition(Vector2f(WIDTH * 0.5 - 155, 150.0f));
	m_textFields["a"].setOrigin(m_textFields["a"].getSize() * 0.5f);
	m_textFields["a"].setFillColor(Vector4f(224.0f / 255.0f, 165.0f / 255.0f, 0.0f, 70.0f / 255.0f));
	m_textFields["a"].setOutlineColor(Vector4f(224.0f / 255.0f, 165.0f / 255.0f, 0.0f, 1.0f));
	m_textFields["a"].setOutlineThickness(4.0f);
	m_textFields["a"].setTextColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

	m_textFields["s"].setPosition(Vector2f(WIDTH * 0.5 - 55.0f, 150.0f));
	m_textFields["s"].setOrigin(m_textFields["s"].getSize() * 0.5f);
	m_textFields["s"].setFillColor(Vector4f(224.0f / 255.0f, 165.0f / 255.0f, 0.0f, 70.0f / 255.0f));
	m_textFields["s"].setOutlineColor(Vector4f(224.0f / 255.0f, 165.0f / 255.0f, 0.0f, 1.0f));
	m_textFields["s"].setOutlineThickness(4.0f);
	m_textFields["s"].setTextColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

	m_textFields["d"].setPosition(Vector2f(WIDTH * 0.5 + 45.0f, 150.0f));
	m_textFields["d"].setOrigin(m_textFields["d"].getSize() * 0.5f);
	m_textFields["d"].setFillColor(Vector4f(224.0f / 255.0f, 165.0f / 255.0f, 0.0f, 70.0f / 255.0f));
	m_textFields["d"].setOutlineColor(Vector4f(224.0f / 255.0f, 165.0f / 255.0f, 0.0f, 1.0f));
	m_textFields["d"].setOutlineThickness(4.0f);
	m_textFields["d"].setTextColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
}

void Settings::initButton() {
	m_button = Button("BACK", Vector4f(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 80.0f / 255.0f));
	m_button.setPosition(Vector2f(180.0f, 80.0f));
	m_button.setOrigin(m_button.getSize() * 0.5f);
	m_button.setOutlineThickness(4.0f);

	Transition& transition = Transition::get();

	m_button.setFunction([&]() {
		transition.setFunction([&]() {
			m_isRunning = false;
			transition.start(Mode::Unveil);
		});
		transition.start(Mode::Veil);
	});
}

void Settings::initTimer() {
	m_textAnimTimer.setFunction(0.20f, [&]() {
		constexpr float pos[10] ={
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
		m_texts.at("title").setPosition(Vector2f(WIDTH / 2.0f, HEIGHT - pos[m_iterator]));

		m_iterator++;
		if (m_iterator > 9)
			m_iterator = 0;
	});
}

void Settings::animateText() {
	m_textAnimTimer.update(m_dt);
}

