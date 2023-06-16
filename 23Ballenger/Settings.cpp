#include "Settings.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"

Settings::Settings(StateMachine& machine) : State(machine, CurrentState::SETTINGS) {

	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);

	m_headline.setCharset(Globals::fontManager.get("upheaval_200"));
	m_headline.setPosition(Vector2f(static_cast<float>(Application::Width / 2 - 490), static_cast<float>(Application::Height - 200)));
	m_headline.setOutlineThickness(5.0f);
	m_headline.setText("Settings");
	m_headline.setOffset(5.0f, 20.0f);
	m_headline.setShader(Globals::shaderManager.getAssetPointer("quad_color_uniform"));

	m_button.setCharset(Globals::fontManager.get("upheaval_50"));
	m_button.setPosition(static_cast<float>(Application::Width - 350), 100.0f);
	m_button.setSize(Vector2f(100.0f, 50.0f));
	m_button.setOutlineThickness(5.0f);
	m_button.setText("Menu");
	m_button.setOffset(2.0f, 7.0f);
	m_button.setShader(Globals::shaderManager.getAssetPointer("quad_color_uniform"));
	m_button.setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	});
}

Settings::~Settings() {
	EventDispatcher::RemoveMouseListener(this);
	EventDispatcher::RemoveKeyboardListener(this);
}

void Settings::fixedUpdate() {}

void Settings::update() {}

void Settings::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_headline.draw();
	m_button.draw();
}

void Settings::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_button.processInput(event.x, Application::Height - event.y);
}

void Settings::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_button.processInput(event.x, Application::Height - event.y, event.button);
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
}