#pragma once

#include "StateMachine.h"
#include "AssetManger.h"

#include "Transition.h"
#include "Button.h"
#include "SeekerBar.h"
#include "ParticleEmitter.h"
#include "Text_old.h"

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
	ParticleEmitter* m_emitter;

	Button m_button;
	std::unordered_map<std::string, unsigned int> m_sprites;
	std::unordered_map<std::string, SeekerBar*> m_seekerBars;
	std::unordered_map<std::string, Text> m_texts;
	void initSprites();
	void initTexts();

	void updateSeekerBars();
	void initSeekerBars();

};