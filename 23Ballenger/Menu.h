#pragma once
#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"
#include "engine/input/Mouse.h"
#include "engine/Transform.h"
#include "StateMachine.h"
#include "Button.h"
#include "TextField.h"

class Menu : public State, public MouseEventListener, public KeyboardEventListener {

public:
	Menu(StateMachine& machine);
	~Menu();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

private:

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;

	TextField m_headline;
	std::unordered_map<std::string, Button> m_buttons;
};