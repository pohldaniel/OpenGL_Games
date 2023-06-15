#include "Settings.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"

Settings::Settings(StateMachine& machine) : State(machine, CurrentState::SETTINGS) {
	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);

	m_button.setPosition(Vector2f(200.0f, 200.0f));
	m_button.setSize(Vector2f(100.0f, 50.0f));
	m_button.setOutlineThickness(5.0f);

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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_button.draw();
}

void Settings::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_button.processInput(event.x, Application::Height - event.y);
}

void Settings::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_button.processInput(event.x, Application::Height - event.y, event.button);
}

void Settings::OnKeyDown(Event::KeyboardEvent& event) {

}