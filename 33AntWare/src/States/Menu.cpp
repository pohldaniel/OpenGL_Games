#include <engine/Fontrenderer.h>
#include <States/Settings.h>
#include <States/Controls.h>
#include <States/Default.h>

#include "Menu.h"
#include "Application.h"
#include "Globals.h"

Menu::Menu(StateMachine& machine) : State(machine, States::MENU) {

	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);

	m_headline.setCharset(Globals::fontManager.get("upheaval_200"));
	m_headline.setPosition(Vector2f(static_cast<float>(Application::Width / 2 - 220), static_cast<float>(Application::Height - 200)));
	m_headline.setOutlineThickness(5.0f);
	m_headline.setText("Menu");
	m_headline.setOffset(5.0f, -7.0f);

	m_buttons = std::initializer_list<std::pair<const std::string, Button>>({
		{ "tower",	  Button() },
		{ "flow",  Button() },
		{ "bridge",	  Button() }
	});

	m_buttons.at("tower").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("tower").setPosition(50.0f, 250.0f);
	m_buttons.at("tower").setOutlineThickness(5.0f);
	m_buttons.at("tower").setText("Settings");
	m_buttons.at("tower").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Settings(m_machine));
	});

	m_buttons.at("flow").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("flow").setPosition(50.0f, 150.0f);
	m_buttons.at("flow").setOutlineThickness(5.0f);
	m_buttons.at("flow").setText("Controls");
	m_buttons.at("flow").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Controls(m_machine));
	});

	m_buttons.at("bridge").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("bridge").setPosition(50.0f, 50.0f);
	m_buttons.at("bridge").setOutlineThickness(5.0f);
	m_buttons.at("bridge").setText("Default");
	m_buttons.at("bridge").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Default(m_machine));
	});

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

Menu::~Menu() {
	EventDispatcher::RemoveMouseListener(this);
	EventDispatcher::RemoveKeyboardListener(this);
}

void Menu::fixedUpdate() {}

void Menu::update() {}

void Menu::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	m_headline.draw();

	for (auto& b : m_buttons)
		b.second.draw();

}

void Menu::OnMouseMotion(Event::MouseMoveEvent& event) {
	for (auto& b : m_buttons)
	  b.second.processInput(event.x, Application::Height - event.y);
}

void Menu::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	for (auto& b : m_buttons)
	  b.second.processInput(event.x, Application::Height - event.y, event.button);
}

void Menu::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void Menu::resize(int deltaW, int deltaH) {
	m_headline.setPosition(Vector2f(static_cast<float>(Application::Width / 2 - 220), static_cast<float>(Application::Height - 200)));
}