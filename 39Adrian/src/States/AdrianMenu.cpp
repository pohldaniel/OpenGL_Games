#include <engine/Fontrenderer.h>

#include <States/Menu.h>
#include <States/Adrian.h>

#include "AdrianMenu.h"
#include "Application.h"
#include "Globals.h"

AdrianMenu::AdrianMenu(StateMachine& machine) : State(machine, States::ADRIAN_MENU) {

	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);

	m_headline.setCharset(Globals::fontManager.get("upheaval_100"));
	m_headline.setPosition(Vector2f(static_cast<float>(Application::Width / 2 - 320), static_cast<float>(Application::Height - 200)));
	m_headline.setOutlineThickness(5.0f);
	m_headline.setText("Adrian Menu");
	m_headline.setOffset(5.0f, -7.0f);

	m_buttons = std::initializer_list<std::pair<const std::string, Button>>({
		{ "adrian",            Button() }
	});

	
	m_buttons.at("adrian").setCharset(Globals::fontManager.get("upheaval_30"));
	m_buttons.at("adrian").setPosition(50.0f, 100.0f);
	m_buttons.at("adrian").setOutlineThickness(5.0f);
	m_buttons.at("adrian").setText("Adrian");
	m_buttons.at("adrian").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Adrian(m_machine));
	});

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

AdrianMenu::~AdrianMenu() {
	EventDispatcher::RemoveMouseListener(this);
	EventDispatcher::RemoveKeyboardListener(this);
}

void AdrianMenu::fixedUpdate() {}

void AdrianMenu::update() {
	m_headline.processInput(0, 0);
}

void AdrianMenu::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_headline.draw();

	for (auto&& b : m_buttons)
		b.second.draw();

}

void AdrianMenu::OnMouseMotion(Event::MouseMoveEvent& event) {
	for (auto&& b : m_buttons)
		b.second.processInput(event.x, Application::Height - event.y);
}

void AdrianMenu::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	for (auto&& b : m_buttons)
		b.second.processInput(event.x, Application::Height - event.y, event.button);
}

void AdrianMenu::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void AdrianMenu::resize(int deltaW, int deltaH) {
	m_headline.setPosition(Vector2f(static_cast<float>(Application::Width / 2 - 220), static_cast<float>(Application::Height - 200)));
}

void AdrianMenu::OnReEnter(unsigned int prevState) {
	auto shader = Globals::shaderManager.getAssetPointer("font");
	shader->use();
	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f));
	shader->unuse();
}