#pragma once
#include "engine/input/EventDispatcher.h"
#include "engine/input/Mouse.h"

#include "StateMachine.h"
#include "LoadingScreen.h"
#include "Editor.h"
#include "Game.h"
#include "Dialog.h"
#include "Label.h"

class MainMenu : public State {
public:
	MainMenu(StateMachine& machine);
	~MainMenu();

	void fixedUpdate() override;
	void update() override;
	void render(unsigned int &frameBuffer) override;
	void resize(int deltaW, int deltaH) override;

	Dialog m_dialog;

	std::vector<TextureRect> m_interfacetexture;
};