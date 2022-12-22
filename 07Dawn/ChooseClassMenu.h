#pragma once

#include "engine/input/MouseEventListener.h"

#include "StateMachine.h"
#include "LoadingScreen.h"
#include "Dialog.h"

class ChooseClassMenu : public State, public MouseEventListener {

public:
	ChooseClassMenu(StateMachine& machine);
	~ChooseClassMenu();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

private:


	Dialog m_dialog;

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
};