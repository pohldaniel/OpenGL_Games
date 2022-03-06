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
	std::unordered_map<std::string, unsigned int> m_Sprites;

	AssetManager<Texture> m_TextureManager;
	Quad *m_quad;
	Shader *m_shader;

	void initSprites();
	void initAssets();

	Button* m_button1;
	Button* m_button2;
	Button* m_button3;
	Text* m_text;
};