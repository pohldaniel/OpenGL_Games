#pragma once

#include <math.h>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/input/Mouse.h>
#include <engine/Transform.h>
#include <UI/Button.h>

#include <States/StateMachine.h>

class Controls : public State, public MouseEventListener, public KeyboardEventListener {

public:
	Controls(StateMachine& machine);
	~Controls();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

private:

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;

	Button m_button;
	TextField m_headline;
	std::unordered_map<std::string, TextField> m_textFields;
};