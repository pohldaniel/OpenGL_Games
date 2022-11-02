#pragma once

#include "engine/input/Mouse.h"
#include "engine/input/KeyBorad.h"
#include "engine/Batchrenderer.h"
#include "engine/Fontrenderer.h"
#include "TextureManager.h"
#include "Spells.h"
#include "Actions.h"
#include "Tooltip.h"
#include "Constants.h"

class Character;

struct sButton {
	CSpellActionBase* action;
	SpellTooltip* tooltip;
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
		tooltip = NULL;
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
	Tooltip* tooltip;
	int posX;
	int posY;
	int width;
	int height;

	sSpellSlot(int posX_, int posY_, int width_, int height_){
		posX = posX_;
		posY = posY_;
		width = width_;
		height = height_;
		action = NULL;
		tooltip = NULL;
	}
};

struct sDamageDisplay {
	int digitToDisplay;
	bool critical;
	uint8_t damageType;
	int x_pos;
	int y_pos;
	float transparency;
	bool update;
	Clock timer;

	sDamageDisplay(int digitToDisplay_, bool critical_, uint8_t damageType_, int x_pos_, int y_pos_, bool update_) {
		digitToDisplay = digitToDisplay_;
		critical = critical_;
		damageType = damageType_;
		x_pos = x_pos_;
		y_pos = y_pos_;
		transparency = 1.0f;
		update = update_;
	}
};

struct sTextLine {
	std::string text;
	Vector4f color;

	sTextLine(std::string text_, GLfloat color_[]){
		text = text_;
		color[0] = color_[0];
		color[1] = color_[1];
		color[2] = color_[2];
		color[3] = 1.0f;
	}
};



class Interface {
public:

	void loadTextures();
	void init();
	bool enabled;
	void DrawInterface();
	void DrawCursor(bool drawInGameCursor);
	void DrawFloatingSpell();
	void processInput();
	void processInputRightDrag();
	void SetPlayer(Player* player);
	void drawTargetedNPCText();
	void executeSpellQueue();

	void drawCombatText();
	void drawCharacterStates();
	void addCombatText(int amount, bool critical, unsigned char damageType, int x_pos, int y_pos, bool update);
	void drawSpellTooltip(int x, int y);

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
	void dragSpell(sButton* spellQueue);
	int8_t getMouseOverButtonId(int x, int y);
	void resize();
	bool isPreparingAoESpell() const;
	void makeReadyToCast(int x, int y);
	CSpellActionBase* getCurrentAction();
	int buttonId;
	void addTextToLog(std::string text, GLfloat color[]);
	void clearLogWindow();

	static inline std::string ConvertTime(uint16_t seconds) {
		// break our seconds down into hours, minutes or seconds and return a pretty string.
		std::string output_string;
		//std::string returnString;
		std::stringstream ss;

		uint16_t minutes = 0;
		uint16_t hours = 0;

		hours = floor((float)seconds / 3600);
		minutes = ceil((float)seconds / 60) - floor((float)seconds / 3600) * 60;

		if (minutes == 1) { minutes = 0; } // when we're below or at 60 seconds, dont display minutes.

		if (hours > 0) {
			if (hours == 1) {
				ss << hours << " hour ";
			}
			else {
				ss << hours << " hours ";
			}
		}

		if (minutes > 0) {
			ss << minutes << " minutes ";
		}

		if (minutes <= 0 && hours <= 0) {
			if (seconds == 1) {
				ss << seconds << " second";
			}
			else {
				ss << seconds << " seconds";
			}
		}

		output_string = ss.str();
		return output_string;
	}

	static inline std::string ConvertTime(uint16_t seconds, uint16_t duration) {
		// break our seconds down into hours, minutes or seconds and return a pretty string.
		std::string output_string;
		std::stringstream ss;

		uint16_t minutes = 0;
		uint16_t hours = 0;

		seconds = duration - seconds;
		hours = floor((float)seconds / 3600);
		minutes = ceil((float)seconds / 60) - floor((float)seconds / 3600) * 60;

		if (minutes == 1) { minutes = 0; } // when we're below or at 60 seconds, dont display minutes.

		if (hours > 0) {
			ss << hours << "h ";
		}

		if (minutes > 0) {
			ss << minutes << "m ";
		}

		if (minutes <= 0 && hours <= 0) {
			ss << seconds << "s ";
		}

		output_string = ss.str();
		return output_string;
	}

private:
	Interface();

	std::vector<TextureRect> m_interfacetexture;

	std::vector<TextureRect> m_damageDisplayTexturesSmall;
	std::vector<TextureRect> m_damageDisplayTexturesBig;
	unsigned int m_textureAtlas;

	bool isButtonUsed(sButton *button) const;
	bool isSpellUseable(CSpellActionBase* action);
	bool execute;
	Player* player;
	CharacterSet* shortcutFont;
	CharacterSet* cooldownFont;
	CharacterSet* interfaceFont;

	sButton* spellQueue;
	

	std::vector<sDamageDisplay> damageDisplay;

	//GLFT_Font* NPCTextFont;
	//GLFT_Font* levelFont;
	int actionBarPosX;
	int actionBarPosY;
	static Interface s_instance;
	int cursorRadius;
	std::pair<int, int> m_lastMouseDown;
	bool preparingAoESpell;
	bool isMouseOver(int x, int y);
	std::vector<CSpellActionBase*> cooldownSpells;
	std::vector<CSpellActionBase*> activeSpells;

	CSpellActionBase* getSpellAtMouse(int mouseX, int mouseY);
	Tooltip* tooltip;
	std::vector <sTextLine> textDatabase;
};