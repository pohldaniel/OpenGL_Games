#include <engine/Fontrenderer.h>

#include "JellyIntroNew.h"
#include "Application.h"
#include "Globals.h"

JellyIntroNew::JellyIntroNew(StateMachine& machine) : State(machine, CurrentState::JELLYINTRO) {

	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);

	
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
}

JellyIntroNew::~JellyIntroNew() {
	EventDispatcher::RemoveMouseListener(this);
	EventDispatcher::RemoveKeyboardListener(this);
}

void JellyIntroNew::fixedUpdate() {}

void JellyIntroNew::update() {}

void JellyIntroNew::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*m_headline.draw();

	for (auto& b : m_buttons)
		b.second.draw();*/

	glEnable(GL_BLEND);
	Globals::spritesheetManager.getAssetPointer("jelly_font")->bind(0);
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_64"), static_cast<float>(Application::Width / 2 - 465), static_cast<float>(Application::Height - 300), "Sound Volume", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_64"), static_cast<float>(Application::Width / 2 - 465), static_cast<float>(Application::Height - 500), "Music Volume", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Fontrenderer::Get().drawBuffer();
	Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);
	glDisable(GL_BLEND);

}

void JellyIntroNew::OnMouseMotion(Event::MouseMoveEvent& event) {
	
}

void JellyIntroNew::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	
}

void JellyIntroNew::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void JellyIntroNew::resize(int deltaW, int deltaH) {
	
}