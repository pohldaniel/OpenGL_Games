#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/input/Mouse.h>
#include <engine/ui/Button.h>
#include <engine/ui/TextField.h>
#include <engine/Transform.h>

#include <States/StateMachine.h>

class Menu : public State, public MouseEventListener, public KeyboardEventListener {

public:
	Menu(StateMachine& machine);
	~Menu();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnReEnter(unsigned int prevState) override;

private:

	void OnMouseMotion(const Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(const Event::MouseButtonEvent& event) override;
	void OnKeyDown(const Event::KeyboardEvent& event) override;

	TextField m_headline;
	std::unordered_map<std::string, Button> m_buttons;
};