#pragma once

#include "Fontrenderer.h"

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
	Vector2f m_editorFocus;
	int m_tilepos”ffset;
	unsigned int m_tilepos, m_currentTilepos;

	void incTilepos();
	void decTilepos();
};