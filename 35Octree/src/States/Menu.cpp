#include <engine/Fontrenderer.h>
#include <States/Grass/GrassComp.h>
#include <States/Grass/GrassGeom.h>
#include <States/Stencil/Stencil1.h>
#include <States/Shadow/Shadow.h>
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
		{ "grass_comp",  Button() },
		{ "grass_geom",  Button() },
		{ "shadow",      Button() },	
		{ "stencil",   Button() },	
		//{ "interleaved", Button() }
	});

	m_buttons.at("grass_comp").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("grass_comp").setPosition(50.0f, 450.0f);
	m_buttons.at("grass_comp").setOutlineThickness(5.0f);
	m_buttons.at("grass_comp").setText("Grass Comp");
	m_buttons.at("grass_comp").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new GrassComp(m_machine));
	});

	m_buttons.at("grass_geom").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("grass_geom").setPosition(50.0f, 350.0f);
	m_buttons.at("grass_geom").setOutlineThickness(5.0f);
	m_buttons.at("grass_geom").setText("Grass Geom");
	m_buttons.at("grass_geom").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new GrassGeom(m_machine));
	});

	m_buttons.at("shadow").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("shadow").setPosition(50.0f, 250.0f);
	m_buttons.at("shadow").setOutlineThickness(5.0f);
	m_buttons.at("shadow").setText("Shadow");
	m_buttons.at("shadow").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Shadow(m_machine));
	});

	m_buttons.at("stencil").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("stencil").setPosition(50.0f, 150.0f);
	m_buttons.at("stencil").setOutlineThickness(5.0f);
	m_buttons.at("stencil").setText("Stencil");
	m_buttons.at("stencil").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Stencil1(m_machine));
	});

	/*m_buttons.at("interleaved").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("interleaved").setPosition(50.0f, 50.0f);
	m_buttons.at("interleaved").setOutlineThickness(5.0f);
	m_buttons.at("interleaved").setText("Interleaved");
	m_buttons.at("interleaved").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Default(m_machine));
	});*/

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