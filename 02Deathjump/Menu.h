#pragma once

#include "StateMachine.h"
#include "AssetManger.h"

#include "Transition.h"
#include "Game.h"
#include "Settings.h"
#include "Button.h"
#include "TextField.h"

class Menu : public State {
public:
	Menu(StateMachine& machine);
	~Menu();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &m_frameBuffer) override;

private:
	Shader *m_shader;
	Quad *m_quad;
	
	Timer m_textAnimTimer;
	int m_iterator = 0;

	std::unordered_map<std::string, unsigned int> m_sprites;
	std::unordered_map<std::string, Button> m_buttons;
	Text m_text;
	TextField m_bestTime;

	void initSprites();
	void initText();
	void initButtons();
	void initTextField();
	void initTimer();
	void animateText();	
};