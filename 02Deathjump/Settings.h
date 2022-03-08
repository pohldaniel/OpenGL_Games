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
	std::unordered_map<std::string, unsigned int> m_Sprites;

	AssetManager<Texture> m_TextureManager;
	Quad *m_quad;
	Shader *m_shader;
	Shader *m_shaderBlur;

	void initSprites();
	void initAssets();

	Button* m_button1;
	Text* m_text;

	ParticleEmitter* m_emitter;
};