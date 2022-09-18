#pragma once

#include "engine/input/Mouse.h"

#include "StateMachine.h"

#include "Game.h"
#include "Dialog.h"
#include "Label.h"

class MainMenu : public State {
public:
	MainMenu(StateMachine& machine);
	~MainMenu();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &frameBuffer) override;

	Dialog m_dialog;
};