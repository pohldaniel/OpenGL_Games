#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"
#include "engine/input/Mouse.h"
#include "engine/Transform.h"
#include "StateMachine.h"

class MainMenu : public State, public MouseEventListener, public KeyboardEventListener {

public:
	MainMenu(StateMachine& machine);
	~MainMenu();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void processInput();

private:

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;

	Transform m_transform;
	bool m_highlight = false;
};