#pragma once

#include "engine/input/KeyBorad.h"
#include "engine/Batchrenderer.h"
#include "engine/Fontrenderer.h"
#include "TextureManager.h"
#include "SpellActionBase.h"

class Character;

struct sButton {
	CSpellActionBase* action;
	//spellTooltip* tooltip;
	std::string number;
	int posX;
	int posY;
	uint8_t height;
	uint8_t width;
	bool wasPressed;
	int actionSpecificXPos;
	int actionSpecificYPos;
	bool actionReadyToCast;
	bool areaOfEffectOnSpecificLocation;
	Keyboard::Key key;

	sButton(int posX_, int posY_, int width_, int height_, std::string number_, Keyboard::Key key_) {
		posX = posX_;
		posY = posY_;
		height = height_;
		width = width_;
		number = number_;
		key = key_;
		//tooltip = NULL;
		action = NULL;
		actionSpecificXPos = 0;
		actionSpecificYPos = 0;
		actionReadyToCast = true;
		areaOfEffectOnSpecificLocation = false;
		wasPressed = false;
	};
};

struct sSpellSlot{
	CSpellActionBase* action;
	//GLFT_Font* font;
	//Tooltip* tooltip;
	int posX;
	int posY;
	int width;
	int height;

	//void initFont();

	sSpellSlot(int posX_, int posY_, int width_, int height_){
		posX = posX_;
		posY = posY_;
		width = width_;
		height = height_;
		action = NULL;
		//font = NULL;
		//tooltip = NULL;
		//initFont();
	}
};


class Interface{
public:

	void loadTextures();
	void init();
	bool enabled;
	void DrawInterface();
	void DrawCursor(bool drawInGameCursor);
	void DrawFloatingSpell();
	void processInput();
	void SetPlayer(Character *player_);
	void drawTargetedNPCText();

	/*void drawCombatText();
	void drawCharacterStates();*/
	//void SetPlayer(CCharacter* player_);

	//void addCombatText(int amount, bool critical, unsigned short damageType, int x_pos, int y_pos, bool update);

	static Interface& Get();

	void bindActionToButtonNr(int buttonNr, CSpellActionBase *action);
	void bindAction(sButton *button, CSpellActionBase* action);
	void unbindAction(sButton *button);

	std::vector <sButton> button;
	sSpellSlot* floatingSpell;
	sSpellSlot& floatingSpellSlot;


	sSpellSlot* getFloatingSpell() const;
	void setFloatingSpell(CSpellActionBase* newFloatingSpell);
	void unsetFloatingSpell();
	bool hasFloatingSpell() const;
	void setSpellQueue(sButton &button, bool actionReadyToCast = true);
	void dragSpell();
	int8_t getMouseOverButtonId(int x, int y);
	void resize();

	CSpellActionBase* getCurrentAction();

private:
	Interface();

	std::vector<TextureRect> m_interfacetexture;
	unsigned int m_textureAtlas;

	void drawCharacterStates();
	bool isButtonUsed(sButton *button) const;

	Character* player;
	CharacterSet* charset;
	sButton* spellQueue;
	//CTexture damageDisplayTexturesBig;
	//CTexture damageDisplayTexturesSmall;

	//std::vector<sDamageDisplay> damageDisplay;



	//GLFT_Font* NPCTextFont;
	//GLFT_Font* levelFont;
	int actionBarPosX;
	int actionBarPosY;
	static Interface s_instance;
};