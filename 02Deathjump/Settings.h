#pragma once

#include "StateMachine.h"
#include "AssetManger.h"

#include "Transition.h"
#include "Button.h"
#include "ParticleEmitter.h"

class Settings : public State {
public:
	Settings(StateMachine& machine);
	~Settings();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &m_frameBuffer) override;

private:
	std::unordered_map<std::string, unsigned int> m_sprites;

	Quad *m_quad;
	Shader *m_shader;
	Shader *m_shaderBlur;

	void initSprites();



	Button m_button;
	Text* m_text;

	ParticleEmitter* m_emitter;
};