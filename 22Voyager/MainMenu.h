#pragma once
#include "engine/input/EventDispatcher.h"
#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "StateMachine.h"

class MainMenu : public State, public MouseEventListener {

public:
	MainMenu(StateMachine& machine);
	~MainMenu();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void processInput();

private:

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
};