#pragma once
#include "engine/Timer.h"
#include "engine/AssetManger.h"

#include "StateMachine.h"
#include "Transition.h"
#include "Button.h"
#include "SeekerBar.h"
#include "TextField.h"
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

	Timer m_textAnimTimer;
	int m_iterator = 0;

	Button m_button;
	std::unordered_map<std::string, unsigned int> m_sprites;
	std::unordered_map<std::string, SeekerBar> m_seekerBars;
	std::unordered_map<std::string, Text> m_texts;
	std::unordered_map<std::string, TextField> m_textFields;

	void initSprites();
	void initTexts();
	void initTextFields();
	void initButton();
	void initSeekerBars();
	void initTimer();
	void animateText();
};