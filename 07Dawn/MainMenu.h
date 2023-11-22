#pragma once
#include "engine/input/EventDispatcher.h"
#include "engine/input/MouseEventListener.h"

#include "StateMachine.h"
#include "LoadingScreen.h"
#include "Editor.h"
#include "Game.h"
#include "Dialog.h"
#include "Label.h"

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

	Dialog m_dialog;

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
};