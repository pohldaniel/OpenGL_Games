#pragma once

#include "StateMachine.h"
#include "AssetManger.h"

#include "Transition.h"
#include "Game.h"
#include "Settings.h"
#include "Button.h"


class Menu : public State {
public:
	Menu(StateMachine& machine);
	~Menu();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &m_frameBuffer) override;

private:
	std::unordered_map<std::string, unsigned int> m_sprites;

	Quad *m_quad;
	Shader *m_shader;

	void initSprites();

	std::unordered_map<std::string, Button> m_buttons;
	Text* m_text;
};