#include <engine/Fontrenderer.h>

#include <States/Settings.h>
#include <States/Controls.h>
#include <States/Default.h>
#include <States/MonsterHunter.h>
#include <States/Tmx.h>
#include <States/Bars.h>

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
		{ "tmx",            Button() },
		{ "bars",           Button() },
		{ "hunter",         Button() }
	});

	m_buttons.at("tmx").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("tmx").setPosition(50.0f, 350.0f);
	m_buttons.at("tmx").setOutlineThickness(5.0f);
	m_buttons.at("tmx").setText("TMX");
	m_buttons.at("tmx").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Tmx(m_machine));
	});

	m_buttons.at("bars").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("bars").setPosition(50.0f, 250.0f);
	m_buttons.at("bars").setOutlineThickness(5.0f);
	m_buttons.at("bars").setText("Bars");
	m_buttons.at("bars").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Bars(m_machine));
		});


	m_buttons.at("hunter").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("hunter").setPosition(50.0f, 150.0f);
	m_buttons.at("hunter").setOutlineThickness(5.0f);
	m_buttons.at("hunter").setText("Monster Hunter");
	m_buttons.at("hunter").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new MonsterHunter(m_machine));
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