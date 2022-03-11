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
	
	Shader *m_shader;
	Shader *m_shaderBlur;

	Quad *m_quad;	
	Text* m_text;
	ParticleEmitter* m_emitter;

	std::unordered_map<std::string, unsigned int> m_sprites;
	Button m_button;

	void initSprites();
	
};