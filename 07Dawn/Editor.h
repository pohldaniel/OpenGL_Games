#pragma once

#include "Fontrenderer.h"
#include "Luainterface.h"
#include "StateMachine.h"

#include "Character.h"

class Editor : public State {
public:
	Editor(StateMachine& machine);
	~Editor();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &frameBuffer) override;
	void initTextures();
	void loadNPCs();

	Zone* newZone;

	unsigned int m_textureAtlas;
	std::vector<TextureRect> m_interfacetexture;
	Vector2f m_editorFocus;
	Vector2f m_originalFocus;
	int m_tileposOffset, m_objectEditSelected;
	unsigned int m_tilepos, m_currentTilepos;
	
	std::vector<std::pair<std::string, CharacterType> > editorNPCs;
	TileClassificationType::TileClassificationType m_selectedTileSet;

	void incTilepos();
	void decTilepos();

private:
	void printShortText(const CharacterSet& characterSet, const std::string &printText, int left, int width, int bottom, int height);
};