#pragma once

#include "StateMachine.h"

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

	std::unordered_map<std::string, unsigned int> m_sprites;

	void initSprites();
};