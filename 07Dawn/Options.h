#pragma once
#include "engine/input/EventDispatcher.h"
#include "engine/input/Mouse.h"

#include "StateMachine.h"
#include "LoadingScreen.h"
#include "Editor.h"
#include "Game.h"
#include "Dialog.h"
#include "Label.h"

class Options : public State {

public:
	Options(StateMachine& machine);
	~Options();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

private:

	std::vector<TextureRect> m_textures;
	unsigned int m_textureAtlas;
	CharacterSet* m_font;
	Dialog m_dialog;
	std::vector<DEVMODE> m_screenModes;
	int m_selected;
};