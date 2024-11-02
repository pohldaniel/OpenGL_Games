#include <engine/Fontrenderer.h>
#include <States/Menu.h>

#include "Controls.h"
#include "Application.h"
#include "Globals.h"

Controls::Controls(StateMachine& machine) : State(machine, States::CONTROLLS) {

	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);

	m_headline.setCharset(Globals::fontManager.get("upheaval_200"));
	m_headline.setPosition(Vector2f(static_cast<float>(Application::Width / 2 - 490), static_cast<float>(Application::Height - 200)));
	m_headline.setOutlineThickness(5.0f);
	m_headline.setText("Controls");
	m_headline.setOffset(5.0f, -7.0f);

	m_button.setCharset(Globals::fontManager.get("upheaval_50"));
	m_button.setPosition(static_cast<float>(Application::Width - 350), 100.0f);
	//m_button.setSize(Vector2f(70.0f, 50.0f));
	m_button.setOutlineThickness(5.0f);
	m_button.setText("Menu");
	//m_button.setOffset(2.0f, 7.0f);

	m_button.setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	});

	m_textFields = std::initializer_list<std::pair<const std::string, TextField>>({
		{ "w", TextField() },
		{ "a", TextField() },
		{ "s", TextField() },
		{ "d", TextField() },
		{ "mouse", TextField() },
		{ "f1", TextField() },
		{ "rm", TextField() },
		{ "alt", TextField() },
		{ "enter", TextField() },
		{ "space", TextField() },
		{ "wheel", TextField() }
	});

	m_textFields.at("w").setCharset(Globals::fontManager.get("upheaval_50"));
	m_textFields.at("w").setPosition(static_cast<float>(Application::Width / 2 - 665), static_cast<float>(Application::Height - 400));
	m_textFields.at("w").setOutlineThickness(5.0f);
	m_textFields.at("w").setText("w");

	m_textFields.at("a").setCharset(Globals::fontManager.get("upheaval_50"));
	m_textFields.at("a").setPosition(static_cast<float>(Application::Width / 2 - 565), static_cast<float>(Application::Height - 400));
	m_textFields.at("a").setOutlineThickness(5.0f);
	m_textFields.at("a").setText("a");

	m_textFields.at("s").setCharset(Globals::fontManager.get("upheaval_50"));
	m_textFields.at("s").setPosition(static_cast<float>(Application::Width / 2 - 465), static_cast<float>(Application::Height - 400));
	m_textFields.at("s").setOutlineThickness(5.0f);
	m_textFields.at("s").setText("s");

	m_textFields.at("d").setCharset(Globals::fontManager.get("upheaval_50"));
	m_textFields.at("d").setPosition(static_cast<float>(Application::Width / 2 - 365), static_cast<float>(Application::Height - 400));
	m_textFields.at("d").setOutlineThickness(5.0f);
	m_textFields.at("d").setText("d");

	m_textFields.at("space").setCharset(Globals::fontManager.get("upheaval_50"));
	m_textFields.at("space").setPosition(static_cast<float>(Application::Width / 2 - 265), static_cast<float>(Application::Height - 400));
	m_textFields.at("space").setOutlineThickness(5.0f);
	m_textFields.at("space").setText("Space");

	m_textFields.at("mouse").setCharset(Globals::fontManager.get("upheaval_50"));
	m_textFields.at("mouse").setPosition(static_cast<float>(Application::Width / 2 - 665), static_cast<float>(Application::Height - 800));
	m_textFields.at("mouse").setOutlineThickness(5.0f);
	m_textFields.at("mouse").setText("Mouse");

	m_textFields.at("rm").setCharset(Globals::fontManager.get("upheaval_50"));
	m_textFields.at("rm").setPosition(static_cast<float>(Application::Width / 2 + 50), static_cast<float>(Application::Height - 400));
	m_textFields.at("rm").setOutlineThickness(5.0f);
	m_textFields.at("rm").setText("Mouse Button right");

	m_textFields.at("f1").setCharset(Globals::fontManager.get("upheaval_50"));
	m_textFields.at("f1").setPosition(static_cast<float>(Application::Width / 2 - 665), static_cast<float>(Application::Height - 600));
	m_textFields.at("f1").setOutlineThickness(5.0f);
	m_textFields.at("f1").setText("F1");

	m_textFields.at("wheel").setCharset(Globals::fontManager.get("upheaval_50"));
	m_textFields.at("wheel").setPosition(static_cast<float>(Application::Width / 2 + 420), static_cast<float>(Application::Height - 600));
	m_textFields.at("wheel").setOutlineThickness(5.0f);
	m_textFields.at("wheel").setText("Wheel");

	m_textFields.at("alt").setCharset(Globals::fontManager.get("upheaval_50"));
	m_textFields.at("alt").setPosition(static_cast<float>(Application::Width / 2 - 220), static_cast<float>(Application::Height - 800));
	m_textFields.at("alt").setOutlineThickness(5.0f);
	m_textFields.at("alt").setText("Alt");

	m_textFields.at("enter").setCharset(Globals::fontManager.get("upheaval_50"));
	m_textFields.at("enter").setPosition(static_cast<float>(Application::Width / 2 - 45), static_cast<float>(Application::Height - 800));
	m_textFields.at("enter").setOutlineThickness(5.0f);
	m_textFields.at("enter").setText("Enter");
}

Controls::~Controls() {
	EventDispatcher::RemoveMouseListener(this);
	EventDispatcher::RemoveKeyboardListener(this);
}

void Controls::fixedUpdate() {}

void Controls::update() {}

void Controls::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_headline.draw();
	m_button.draw();
	for (auto&& b : m_textFields)
		b.second.draw();
	

	Globals::fontManager.get("upheaval_50").bind(0);
	Fontrenderer::Get().addText(Globals::fontManager.get("upheaval_50"), static_cast<float>(Application::Width / 2 - 665), static_cast<float>(Application::Height - 300), "Move", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("upheaval_50"), static_cast<float>(Application::Width / 2 +  50), static_cast<float>(Application::Height - 300), "Break", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("upheaval_50"), static_cast<float>(Application::Width / 2 - 665), static_cast<float>(Application::Height - 500), "Jump to last activated respawnpoint", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("upheaval_50"), static_cast<float>(Application::Width / 2 + 420), static_cast<float>(Application::Height - 500), "Zoom", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("upheaval_50"), static_cast<float>(Application::Width / 2 - 665), static_cast<float>(Application::Height - 700), "Look around", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("upheaval_50"), static_cast<float>(Application::Width / 2 - 220), static_cast<float>(Application::Height - 700), "Toggle Fullscreen", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Fontrenderer::Get().drawBuffer();
}

void Controls::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_button.processInput(event.x, Application::Height - event.y);
}

void Controls::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_button.processInput(event.x, Application::Height - event.y, event.button);
}

void Controls::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Controls::resize(int deltaW, int deltaH) {
	m_headline.setPosition(Vector2f(static_cast<float>(Application::Width / 2 - 490), static_cast<float>(Application::Height - 200)));
	m_button.setPosition(static_cast<float>(Application::Width - 350), 100.0f);

	m_textFields.at("w").setPosition(static_cast<float>(Application::Width / 2 - 665), static_cast<float>(Application::Height - 400));
	m_textFields.at("a").setPosition(static_cast<float>(Application::Width / 2 - 565), static_cast<float>(Application::Height - 400));
	m_textFields.at("s").setPosition(static_cast<float>(Application::Width / 2 - 465), static_cast<float>(Application::Height - 400));
	m_textFields.at("d").setPosition(static_cast<float>(Application::Width / 2 - 365), static_cast<float>(Application::Height - 400));
	m_textFields.at("mouse").setPosition(static_cast<float>(Application::Width / 2 - 665), static_cast<float>(Application::Height - 800));
	m_textFields.at("rm").setPosition(static_cast<float>(Application::Width / 2 + 50), static_cast<float>(Application::Height - 400));
	m_textFields.at("f1").setPosition(static_cast<float>(Application::Width / 2 - 665), static_cast<float>(Application::Height - 600));
	m_textFields.at("alt").setPosition(static_cast<float>(Application::Width / 2 - 220), static_cast<float>(Application::Height - 800));
	m_textFields.at("enter").setPosition(static_cast<float>(Application::Width / 2 - 45), static_cast<float>(Application::Height - 800));
	m_textFields.at("space").setPosition(static_cast<float>(Application::Width / 2 - 265), static_cast<float>(Application::Height - 400));
	m_textFields.at("wheel").setPosition(static_cast<float>(Application::Width / 2 + 420), static_cast<float>(Application::Height - 600));
}