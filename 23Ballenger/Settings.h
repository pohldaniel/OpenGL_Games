#pragma once
#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"
#include "engine/input/Mouse.h"
#include "engine/Transform.h"
#include "StateMachine.h"
#include "Button.h"

class Settings : public State, public MouseEventListener, public KeyboardEventListener {

public:
	Settings(StateMachine& machine);
	~Settings();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

private:

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;

	Transform m_transform;
	bool m_highlight = false;
	Vector4f m_color1, m_color2, m_color3;

	Button m_button;
	TextField m_headline;
};