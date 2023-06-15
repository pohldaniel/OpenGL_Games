#include "engine/Fontrenderer.h"

#include "Menu.h"
#include "Application.h"
#include "Globals.h"
#include "Settings.h"
#include "Game.h"
#include "ShapeInterface.h"
#include "TerrainCulling.h"
#include "CloudInterface.h"

Menu::Menu(StateMachine& machine) : State(machine, CurrentState::MENU), 
	m_headline(Globals::fontManager.get("upheaval_200")),
	m_button1(Globals::fontManager.get("upheaval_50")),
	m_button2(Globals::fontManager.get("upheaval_50")),
	m_button3(Globals::fontManager.get("upheaval_50")),
	m_button4(Globals::fontManager.get("upheaval_50")),
	m_button5(Globals::fontManager.get("upheaval_50")) {

	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);

	m_headline.setPosition(Vector2f(static_cast<float>(Application::Width / 2 - 220), static_cast<float>(Application::Height - 200)));
	m_headline.setOutlineThickness(5.0f);
	m_headline.setText("Menu");
	m_headline.setOffset(5.0f, 20.0f);
	m_headline.setShader(Globals::shaderManager.getAssetPointer("quad_color_uniform"));

	m_button1.setPosition(static_cast<float>(Application::Width - 350), 200.0f);
	m_button1.setSize(Vector2f(100.0f, 50.0f));
	m_button1.setOutlineThickness(5.0f);
	m_button1.setText("New Game");
	m_button1.setOffset(2.0f, 7.0f);
	m_button1.setShader(Globals::shaderManager.getAssetPointer("quad_color_uniform"));

	m_button1.setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Game(m_machine));
	});

	m_button2.setPosition(static_cast<float>(Application::Width - 350), 100.0f);
	m_button2.setSize(Vector2f(246.0f, 50.0f));
	m_button2.setOutlineThickness(5.0f);
	m_button2.setText("Settings");
	m_button2.setOffset(2.0f, 7.0f);
	m_button2.setShader(Globals::shaderManager.getAssetPointer("quad_color_uniform"));

	m_button2.setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Settings(m_machine));
	});

	m_button3.setPosition(50.0f, 300.0f);
	m_button3.setSize(Vector2f(100.0f, 50.0f));
	m_button3.setOutlineThickness(5.0f);
	m_button3.setText("Shape Interface");
	m_button3.setOffset(2.0f, 7.0f);
	m_button3.setShader(Globals::shaderManager.getAssetPointer("quad_color_uniform"));

	m_button3.setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new ShapeInterface(m_machine));
	});

	m_button4.setPosition(50.0f, 200.0f);
	m_button4.setSize(Vector2f(449.0f, 50.0f));
	m_button4.setOutlineThickness(5.0f);
	m_button4.setText("Terrain Culling");
	m_button4.setOffset(2.0f, 7.0f);
	m_button4.setShader(Globals::shaderManager.getAssetPointer("quad_color_uniform"));

	m_button4.setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new TerrainCulling(m_machine));
	});

	m_button5.setPosition(50.0f, 100.0f);
	m_button5.setSize(Vector2f(100.0f, 50.0f));
	m_button5.setOutlineThickness(5.0f);
	m_button5.setText("Cloud Interface");
	m_button5.setOffset(2.0f, 7.0f);
	m_button5.setShader(Globals::shaderManager.getAssetPointer("quad_color_uniform"));

	m_button5.setFunction([&]() {
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
	m_button1.draw();
	m_button2.draw();
	m_button3.draw();
	m_button4.draw();
	m_button5.draw();

}

void Menu::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_button1.processInput(event.x, Application::Height - event.y);
	m_button2.processInput(event.x, Application::Height - event.y);
	m_button3.processInput(event.x, Application::Height - event.y);
	m_button4.processInput(event.x, Application::Height - event.y);
	m_button5.processInput(event.x, Application::Height - event.y);
}

void Menu::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_button1.processInput(event.x, Application::Height - event.y, event.button);
	m_button2.processInput(event.x, Application::Height - event.y, event.button);
	m_button3.processInput(event.x, Application::Height - event.y, event.button);
	m_button4.processInput(event.x, Application::Height - event.y, event.button);
	m_button5.processInput(event.x, Application::Height - event.y, event.button);
}

void Menu::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void Menu::resize(int deltaW, int deltaH) {
	m_headline.setPosition(Vector2f(static_cast<float>(Application::Width / 2 - 220), static_cast<float>(Application::Height - 200)));

	m_button1.setPosition(static_cast<float>(Application::Width - 350), 200.0f);
	m_button2.setPosition(static_cast<float>(Application::Width - 350), 100.0f);
	m_button3.setPosition(50.0f, 300.0f);
	m_button4.setPosition(50.0f, 200.0f);
	m_button5.setPosition(50.0f, 100.0f);
}