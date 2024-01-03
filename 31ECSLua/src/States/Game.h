#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>

#include <engine/Rect.h>
#include <ECS/ECS.h>
#include <EventBus/EventBus.h>
#include <sol/sol.hpp>

#include <States/StateMachine.h>
#include "Background.h"

class Game : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Game(StateMachine& machine);
	~Game();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void OnKeyUp(Event::KeyboardEvent& event) override;

private:

	void renderUi();
	void init();

	bool m_initUi = true;
	bool m_drawUi = false;
	bool m_debug = false;

	Camera& camera;
	Background m_background;
	sol::state lua;

	std::unique_ptr<Registry> registry;
	std::unique_ptr<EventBus> eventBus;
};