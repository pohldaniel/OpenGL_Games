#pragma once

#include "StateMachine.h"
#include "Button.h"


class LevelSelect : public State {
public:
	LevelSelect(StateMachine& machine);
	~LevelSelect();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &m_frameBuffer) override;

private:
	Shader *m_shader;
	Quad *m_quadBackground;
	
	std::unordered_map<std::string, unsigned int> m_sprites;
	unsigned long m_guard;
	std::vector<Button*> m_buttons;

	void initSprites();
	
	unsigned int m_blocks;
	unsigned int m_currentBlock = 0;
};