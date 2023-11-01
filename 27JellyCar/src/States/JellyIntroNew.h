#pragma once
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/input/Mouse.h>

#include <States/StateMachine.h>
#include <UI/Button.h>
#include <UI/TextField.h>

class JellyIntroNew : public State, public MouseEventListener, public KeyboardEventListener {

public:
	JellyIntroNew(StateMachine& machine);
	~JellyIntroNew();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

private:

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
};