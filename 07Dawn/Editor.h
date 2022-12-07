#pragma once

#include "engine/Fontrenderer.h"
#include "Luainterface.h"
#include "StateMachine.h"

#include "Zone.h"
#include "InteractionRegion.h"
#include "Character.h"
#include "Message.h"

class Editor : public State {
public:
	Editor(StateMachine& machine);
	~Editor();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render() override;

private:
	void printShortText(const CharacterSet& characterSet, const std::string &printText, int left, int width, int bottom, int height);
	void drawEditFrame(EnvironmentMap* editobject);
	void updateAdjacencyList();
	void placeAdjacentTile();
	void applyAdjacencyModification(int modification);
	void saveZone();
	void initTextures();
	void loadNPCs();
	void incTilepos();
	void decTilepos();

	Zone* newZone;

	unsigned int m_textureAtlas;
	std::vector<TextureRect> m_interfacetexture;
	Vector2f m_editorFocus;
	Vector2f m_originalFocus;
	Vector2f m_currentFocus;

	std::vector<std::pair<std::string, CharacterType>> editorNPCs;
	Enums::TileClassificationType m_selectedTileSet;
	int m_tileposOffset, m_selectedObjectId = -1;
	unsigned int m_tilepos, m_currentTilepos;

	bool adjacencyModeEnabled;
	std::vector<std::vector<Tile>> curAdjacentTiles;
	std::vector<std::vector<std::array<int, 2>>> curAdjacencyOffsets;
	unsigned int curDirectionAdjacencySelection[Enums::AdjacencyType::BOTTOM + 1];
};