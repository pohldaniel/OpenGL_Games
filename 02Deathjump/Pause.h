#pragma once

#include "StateMachine.h"
#include "AssetManger.h"

#include "Button.h"


class Pause : public State {
public:
	Pause(StateMachine& machine);
	~Pause();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &m_frameBuffer) override;

private:
	std::unordered_map<std::string, unsigned int> m_Sprites;

	AssetManager<Texture> m_TextureManager;
	Quad *m_quad;
	Shader *m_shader;
	Shader *m_shaderBlur;

	void InitSprites();
	void InitAssets();

	Button* m_button1;
	Button* m_button2;
	Button* m_button3;
	Text* m_text;
};