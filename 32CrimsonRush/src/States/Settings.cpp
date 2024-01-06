#include "engine/Fontrenderer.h"
#include "Settings.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"

Settings::Settings(StateMachine& machine) : State(machine, States::SETTINGS) {

	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);

	soundVolume = 0.2f;
	musicVolume = 0.1f;
	useSkybox = true;

	m_headline.setCharset(Globals::fontManager.get("upheaval_200"));
	m_headline.setPosition(Vector2f(static_cast<float>(Application::Width / 2 - 490), static_cast<float>(Application::Height - 200)));
	m_headline.setOutlineThickness(5.0f);
	m_headline.setText("Settings");
	m_headline.setOffset(5.0f, -7.0f);

	m_button.setCharset(Globals::fontManager.get("upheaval_50"));
	m_button.setPosition(static_cast<float>(Application::Width - 350), 100.0f);
	m_button.setSize(Vector2f(100.0f, 50.0f));
	m_button.setOutlineThickness(5.0f);
	m_button.setText("Menu");
	m_button.setOffset(2.0f, 7.0f);

	m_button.setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	});

	m_seekerBars = std::initializer_list<std::pair<const std::string, SeekerBar>>({
		{ "effect", SeekerBar() },
		{ "music",  SeekerBar() }
	});

	float currentBlock;

	std::modf(soundVolume * 10, &currentBlock);
	m_seekerBars.at("effect").initButtons(Globals::fontManager.get("upheaval_50"));
	m_seekerBars.at("effect").initRenderer(10u);
	m_seekerBars.at("effect").setPosition(static_cast<float>(Application::Width / 2 - 400), static_cast<float>(Application::Height - 400));
	m_seekerBars.at("effect").setCurrentBlock(static_cast<unsigned int>(currentBlock));
	m_seekerBars.at("effect").setLeftFunction([&]() {
		if (soundVolume > 0.0f) {
			soundVolume -= 0.1f;
			soundVolume = roundf(soundVolume * 10) / 10;
		}
	});

	m_seekerBars.at("effect").setRightFunction([&]() {
		if (soundVolume < 1.0f) {
			soundVolume += 0.1f;
			soundVolume = roundf(soundVolume * 10) / 10;
		}
	});

	std::modf(musicVolume * 10, &currentBlock);
	m_seekerBars.at("music").initButtons(Globals::fontManager.get("upheaval_50"));
	m_seekerBars.at("music").initRenderer(10u);
	m_seekerBars.at("music").setPosition(static_cast<float>(Application::Width / 2 - 400), static_cast<float>(Application::Height - 600));
	m_seekerBars.at("music").setCurrentBlock(static_cast<unsigned int>(currentBlock));
	m_seekerBars.at("music").setLeftFunction([&]() {
		if (musicVolume > 0.0f) {
			musicVolume -= 0.1f;
			musicVolume = roundf(musicVolume * 10) / 10;
		}	
	});

	m_seekerBars.at("music").setRightFunction([&]() {
		if (musicVolume < 1.0f) {
			musicVolume += 0.1f;
			musicVolume = roundf(musicVolume * 10) / 10;
		}
	});

	m_checkBox.setPosition(Vector2f(static_cast<float>(Application::Width / 2 - 465), static_cast<float>(Application::Height - 800)));
	m_checkBox.setSize(52.0f, 52.0f);
	m_checkBox.setOutlineThickness(5.0f);
	m_checkBox.setIsChecked(useSkybox);
	m_checkBox.setFunction([&]() {
		useSkybox = !useSkybox;
	});
}

Settings::~Settings() {
	EventDispatcher::RemoveMouseListener(this);
	EventDispatcher::RemoveKeyboardListener(this);
}

void Settings::fixedUpdate() {}

void Settings::update() {}

void Settings::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_headline.draw();
	m_checkBox.draw();
	m_button.draw();

	for (auto& b : m_seekerBars)
		b.second.draw();

	glEnable(GL_BLEND);
	Globals::fontManager.get("upheaval_50").bind(0);
	Fontrenderer::Get().addText(Globals::fontManager.get("upheaval_50"), static_cast<float>(Application::Width / 2 - 465), static_cast<float>(Application::Height - 300), "Sound Volume", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("upheaval_50"), static_cast<float>(Application::Width / 2 - 465), static_cast<float>(Application::Height - 500), "Music Volume", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("upheaval_50"), static_cast<float>(Application::Width / 2 - 465), static_cast<float>(Application::Height - 700), m_checkBox.isChecked() ? "Use Skybox" : "Use procedural Clouds", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Fontrenderer::Get().drawBuffer();
	glDisable(GL_BLEND);
}

void Settings::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_button.processInput(event.x, Application::Height - event.y);
	m_checkBox.processInput(event.x, Application::Height - event.y);
	for (auto& b : m_seekerBars)
		b.second.processInput(event.x, Application::Height - event.y);
}

void Settings::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_button.processInput(event.x, Application::Height - event.y, event.button);
	m_checkBox.processInput(event.x, Application::Height - event.y, event.button);
	for (auto& b : m_seekerBars)
		b.second.processInput(event.x, Application::Height - event.y, event.button);
}

void Settings::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Settings::resize(int deltaW, int deltaH) {
	m_headline.setPosition(Vector2f(static_cast<float>(Application::Width / 2 - 490), static_cast<float>(Application::Height - 200)));
	m_button.setPosition(static_cast<float>(Application::Width - 350), 100.0f);
	m_seekerBars.at("effect").setPosition(static_cast<float>(Application::Width / 2 - 400), static_cast<float>(Application::Height - 400));
	m_seekerBars.at("music").setPosition(static_cast<float>(Application::Width / 2 - 400), static_cast<float>(Application::Height - 600));
	m_checkBox.setPosition(Vector2f(static_cast<float>(Application::Width / 2 - 450), static_cast<float>(Application::Height - 800)));
}