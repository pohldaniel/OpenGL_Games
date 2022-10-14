#pragma once

#include "engine/Batchrenderer.h"
#include "engine/Fontrenderer.h"
#include "TextureManager.h"

class Character;

class Interface{
public:

	void loadTextures();
	void init();
	bool enabled;
	void DrawInterface();
	void DrawCursor(bool drawInGameCursor);
	void SetPlayer(Character *player_);
	void drawTargetedNPCText();

	/*void drawCombatText();
	void drawCharacterStates();*/
	//void SetPlayer(CCharacter* player_);

	//void addCombatText(int amount, bool critical, unsigned short damageType, int x_pos, int y_pos, bool update);

	static Interface& Get();

private:
	Interface() = default;

	std::vector<TextureRect> m_interfacetexture;
	unsigned int m_textureAtlas;

	void drawCharacterStates();

	Character* player;
	CharacterSet* m_charset;
	//CTexture damageDisplayTexturesBig;
	//CTexture damageDisplayTexturesSmall;

	//std::vector<sDamageDisplay> damageDisplay;



	//GLFT_Font* NPCTextFont;
	//GLFT_Font* levelFont;

	static Interface s_instance;
};