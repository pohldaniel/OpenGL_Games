#include <engine/Fontrenderer.h>

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
	m_headline.setOffset(5.0f, 5.0f);

	m_buttons = std::initializer_list<std::pair<const std::string, Button>>({		
		{ "ragdoll" , Button() },
		{ "vehicle" , Button() },
		{ "character" , Button() }
	});

	m_buttons.at("character").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("character").setPosition(50.0f, 500.0f);
	m_buttons.at("character").setSize(Vector2f(100.0f, 50.0f));
	m_buttons.at("character").setOutlineThickness(5.0f);
	m_buttons.at("character").setText("Character");
	m_buttons.at("character").setOffset(2.0f, 7.0f);


	m_buttons.at("ragdoll").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("ragdoll").setPosition(50.0f, 400.0f);
	m_buttons.at("ragdoll").setSize(Vector2f(100.0f, 50.0f));
	m_buttons.at("ragdoll").setOutlineThickness(5.0f);
	m_buttons.at("ragdoll").setText("Ragdoll");
	m_buttons.at("ragdoll").setOffset(2.0f, 7.0f);


	m_buttons.at("vehicle").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("vehicle").setPosition(50.0f, 300.0f);
	m_buttons.at("vehicle").setSize(Vector2f(100.0f, 50.0f));
	m_buttons.at("vehicle").setOutlineThickness(5.0f);
	m_buttons.at("vehicle").setText("Vehicle");
	m_buttons.at("vehicle").setOffset(2.0f, 7.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

Menu::~Menu() {
	EventDispatcher::RemoveMouseListener(this);
	EventDispatcher::RemoveKeyboardListener(this);
}

void Menu::fixedUpdate() {}

void Menu::update() {}

void Menu::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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