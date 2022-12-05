#pragma once

#include "engine/input/Mouse.h"
#include "engine/input/KeyBorad.h"
#include "engine/Batchrenderer.h"
#include "engine/Fontrenderer.h"
#include "TextureManager.h"
#include "Spellbook.h"
#include "CharacterInfo.h"

#include "Constants.h"

class Character;

struct Button {
	int posX;
	int posY;
	int height;
	int width;
	std::string number;
	Keyboard::Key key;

	SpellActionBase* action = nullptr;
	SpellTooltip* tooltip = nullptr;
	int actionSpecificXPos = 0;
	int actionSpecificYPos = 0;
	bool actionReadyToCast = true;
	bool areaOfEffectOnSpecificLocation = false;

	Button(int _posX, int _posY, int _width, int _height, std::string _number, Keyboard::Key _key) {
		posX = _posX;
		posY = _posY;
		height = _height;
		width = _width;
		number = _number;
		key = _key;		
	};
};

struct DamageDisplay {
	int digitToDisplay;
	bool critical;
	unsigned short damageType;
	int posX;
	int posY;	
	bool update;

	float transparency = 1.0f;
	Clock timer;

	DamageDisplay(int _digitToDisplay, bool _critical, unsigned short _damageType, int _posX, int _posY, bool _update) {
		digitToDisplay = _digitToDisplay;
		critical = _critical;
		damageType = _damageType;
		posX = _posX;
		posY = _posY;
		update = _update;
	}
};

struct TextLine {
	std::string text;
	Vector4f color;
};

class Interface {

public:
	~Interface();
	void draw();
	void drawCursor(bool drawInGameCursor);

	void processInput();
	void processInputRightDrag();

	void init();
	void setPlayer(Player* player);
	void addCombatText(int amount, bool critical, unsigned short damageType, int posX, int posY, bool update);
	void resize(int deltaW, int deltaH);
	void addTextToLog(std::string text, Vector4f color);
	void clearLogWindow();
	void addWidget(Widget& widget, bool activate = false);

	static Interface& Get();
	
private:

	Interface();
	void drawTargetedNPCText();
	void executeSpellQueue();

	void drawCombatText();
	void drawCharacterStates();
	
	void drawSpellTooltip(int x, int y);

	void loadTextures();
	bool isButtonUsed(Button *button) const;
	bool isSpellUseable(SpellActionBase* action);
	bool isMouseOver(int x, int y);
	SpellActionBase* getSpellAtMouse(int mouseX, int mouseY);

	
	void setSpellQueue(Button &button, bool actionReadyToCast = true);
	void dragSpell(Button* spellQueue);
	short getMouseOverButtonId(int x, int y);

	void bindActionToButtonNr(int buttonNr, SpellActionBase *action);
	void bindAction(Button *button, SpellActionBase* action);
	void unbindAction(Button *button);

	bool isPreparingAoESpell() const;
	void makeReadyToCast(int x, int y);

	std::vector<TextLine> m_textDatabase;
	std::vector<DamageDisplay> m_damageDisplay;
	std::vector<SpellActionBase*> m_cooldownSpells;
	std::vector<SpellActionBase*> m_activeSpells;
	std::vector<Button> m_button;

	std::vector<TextureRect> m_interfacetexture;
	std::vector<TextureRect> m_damageDisplayTexturesSmall;
	std::vector<TextureRect> m_damageDisplayTexturesBig;

	unsigned int m_textureAtlas;
	short m_buttonId;
	bool m_execute;
	int m_actionBarPosX;
	int m_actionBarPosY;
	int m_cursorRadius;
	bool m_preparingAoESpell;

	Player* m_player;
	CharacterSet* m_shortcutFont;
	CharacterSet* m_cooldownFont;
	CharacterSet* m_interfaceFont;
	Tooltip* m_tooltip;
	Button* m_spellQueue;

	Spellbook& m_spellbook;
	CharacterInfo& m_characterInfo;


	std::pair<int, int> m_lastMouseDown;
	std::vector <Widget*> m_widgets;
	Widget* m_activeWidget;

	static Interface s_instance;
};