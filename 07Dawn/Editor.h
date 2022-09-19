#pragma once

#include "StateMachine.h"

class Editor : public State {
public:
	Editor(StateMachine& machine);
	~Editor();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &frameBuffer) override;
	void initTextures();

	Zone* newZone;

	unsigned int m_textureAtlas;
	std::vector<DawnTexture> m_interfacetexture;
};