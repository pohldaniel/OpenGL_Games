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
		{ "default", Button()  },
		{ "settings", Button() },
		{ "controls", Button() },
	});

	m_buttons.at("default").setCharset(Globals::fontManager.get("upheaval_30"));
	m_buttons.at("default").setPosition(50.0f, 350.0f);
	m_buttons.at("default").setOutlineThickness(5.0f);
	m_buttons.at("default").setText("Default");
	m_buttons.at("default").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Default(m_machine));
	});

	m_buttons.at("settings").setCharset(Globals::fontManager.get("upheaval_30"));
	m_buttons.at("settings").setPosition(static_cast<float>(Application::Width - 350), 200.0f);
	m_buttons.at("settings").setOutlineThickness(5.0f);
	m_buttons.at("settings").setText("Settings");
	m_buttons.at("settings").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Settings(m_machine));
	});

	m_buttons.at("controls").setCharset(Globals::fontManager.get("upheaval_30"));
	m_buttons.at("controls").setPosition(static_cast<float>(Application::Width - 350), 100.0f);
	m_buttons.at("controls").setOutlineThickness(5.0f);
	m_buttons.at("controls").setText("Controls");
	m_buttons.at("controls").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Controls(m_machine));
	});

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

Menu::~Menu() {
	EventDispatcher::RemoveMouseListener(this);
	EventDispatcher::RemoveKeyboardListener(this);
}

void Menu::fixedUpdate() {}

void Menu::update() {
	m_headline.processInput(0, 0);
}

void Menu::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_headline.draw();

	for (auto&& b : m_buttons)
		b.second.draw();

}

void Menu::OnMouseMotion(Event::MouseMoveEvent& event) {
	for (auto&& b : m_buttons)
		b.second.processInput(event.x, Application::Height - event.y);
}

void Menu::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	for (auto&& b : m_buttons)
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

void Menu::OnReEnter(unsigned int prevState) {
	auto shader = Globals::shaderManager.getAssetPointer("font");
	shader->use();
	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f));
	shader->unuse();
}