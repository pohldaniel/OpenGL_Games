#pragma once

#include "StateMachine.h"
#include "AssetManger.h"

#include "Transition.h"
#include "Settings.h"
#include "Button.h"

class Pause : public State {
public:
	Pause(StateMachine& machine);
	~Pause();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &m_frameBuffer) override;

private:
	Shader *m_shader;
	Shader *m_shaderBlur;

	Quad *m_quad;
	Text* m_text;

	std::unordered_map<std::string, unsigned int> m_sprites;
	std::unordered_map<std::string, Button> m_buttons;

	void initSprites();
};