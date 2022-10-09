#pragma once

#include "engine/Batchrenderer.h"
#include "TextureManager.h"

class Character;

class Interface{
public:
	Interface();
	void LoadTextures();
	void initFonts();
	bool enabled;
	void DrawInterface();
	void DrawCursor(bool drawInGameCursor);
	void SetPlayer(Character *player_);

	/*void drawTargetedNPCText();
	void drawCombatText();
	void drawCharacterStates();*/
	//void SetPlayer(CCharacter* player_);

	//void addCombatText(int amount, bool critical, unsigned short damageType, int x_pos, int y_pos, bool update);

private:
	std::vector<TextureRect> m_interfacetexture;
	unsigned int m_textureAtlas;

	void drawCharacterStates();

	Character* player;

	//CTexture damageDisplayTexturesBig;
	//CTexture damageDisplayTexturesSmall;

	//std::vector<sDamageDisplay> damageDisplay;



	//GLFT_Font* NPCTextFont;
	//GLFT_Font* levelFont;
};