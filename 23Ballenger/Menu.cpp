#include "engine/Fontrenderer.h"

#include "Menu.h"
#include "Application.h"
#include "Globals.h"
#include "Settings.h"
#include "Game.h"
#include "ShapeInterface.h"
#include "TerrainCulling.h"
#include "CloudInterface.h"

Menu::Menu(StateMachine& machine) : State(machine, CurrentState::MENU) {

	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);

	m_headline.setCharset(Globals::fontManager.get("upheaval_200"));
	m_headline.setPosition(Vector2f(static_cast<float>(Application::Width / 2 - 220), static_cast<float>(Application::Height - 200)));
	m_headline.setOutlineThickness(5.0f);
	m_headline.setText("Menu");
	m_headline.setOffset(5.0f, 20.0f);
	m_headline.setShader(Globals::shaderManager.getAssetPointer("quad_color_uniform"));

	m_buttons = std::initializer_list<std::pair<const std::string, Button>>({
		{ "start",	  Button() },
		{ "settings", Button() },
		{ "shape",	  Button() },
		{ "terrain",  Button() },
		{ "cloud",	  Button() }
	});

	m_buttons.at("start").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("start").setPosition(static_cast<float>(Application::Width - 350), 200.0f);
	m_buttons.at("start").setSize(Vector2f(100.0f, 50.0f));
	m_buttons.at("start").setOutlineThickness(5.0f);
	m_buttons.at("start").setText("New Game");
	m_buttons.at("start").setOffset(2.0f, 7.0f);
	m_buttons.at("start").setShader(Globals::shaderManager.getAssetPointer("quad_color_uniform"));
	m_buttons.at("start").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Game(m_machine));
	});

	m_buttons.at("settings").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("settings").setPosition(static_cast<float>(Application::Width - 350), 100.0f);
	m_buttons.at("settings").setSize(Vector2f(246.0f, 50.0f));
	m_buttons.at("settings").setOutlineThickness(5.0f);
	m_buttons.at("settings").setText("Settings");
	m_buttons.at("settings").setOffset(2.0f, 7.0f);
	m_buttons.at("settings").setShader(Globals::shaderManager.getAssetPointer("quad_color_uniform"));
	m_buttons.at("settings").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Settings(m_machine));
	});
	
	m_buttons.at("shape").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("shape").setPosition(50.0f, 300.0f);
	m_buttons.at("shape").setSize(Vector2f(100.0f, 50.0f));
	m_buttons.at("shape").setOutlineThickness(5.0f);
	m_buttons.at("shape").setText("Shape Interface");
	m_buttons.at("shape").setOffset(2.0f, 7.0f);
	m_buttons.at("shape").setShader(Globals::shaderManager.getAssetPointer("quad_color_uniform"));
	m_buttons.at("shape").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new ShapeInterface(m_machine));
	});

	m_buttons.at("terrain").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("terrain").setPosition(50.0f, 200.0f);
	m_buttons.at("terrain").setSize(Vector2f(449.0f, 50.0f));
	m_buttons.at("terrain").setOutlineThickness(5.0f);
	m_buttons.at("terrain").setText("Terrain Culling");
	m_buttons.at("terrain").setOffset(2.0f, 7.0f);
	m_buttons.at("terrain").setShader(Globals::shaderManager.getAssetPointer("quad_color_uniform"));
	m_buttons.at("terrain").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new TerrainCulling(m_machine));
	});

	m_buttons.at("cloud").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("cloud").setPosition(50.0f, 100.0f);
	m_buttons.at("cloud").setSize(Vector2f(100.0f, 50.0f));
	m_buttons.at("cloud").setOutlineThickness(5.0f);
	m_buttons.at("cloud").setText("Cloud Interface");
	m_buttons.at("cloud").setOffset(2.0f, 7.0f);
	m_buttons.at("cloud").setShader(Globals::shaderManager.getAssetPointer("quad_color_uniform"));
	m_buttons.at("cloud").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new CloudInterface(m_machine));
	});
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

	m_buttons.at("start").setPosition(static_cast<float>(Application::Width - 350), 200.0f);
	m_buttons.at("settings").setPosition(static_cast<float>(Application::Width - 350), 100.0f);
	m_buttons.at("shape").setPosition(50.0f, 300.0f);
	m_buttons.at("terrain").setPosition(50.0f, 200.0f);
	m_buttons.at("cloud").setPosition(50.0f, 100.0f);
}