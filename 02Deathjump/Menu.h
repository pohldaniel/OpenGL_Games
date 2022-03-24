#pragma once

#include "StateMachine.h"
#include "AssetManger.h"

#include "Transition.h"
#include "Game.h"
#include "Settings.h"
#include "Button.h"
#include "Text.h"

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
	Text* m_text;
	
	std::unordered_map<std::string, unsigned int> m_sprites;
	std::unordered_map<std::string, Button> m_buttons;
	
	void initSprites();

	bool m_toggle = true;

	

};