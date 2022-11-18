#pragma once
#include "Spells.h"
#include "Actions.h"
#include "Tooltip.h"

struct SpellSlot {
	int posX;
	int posY;
	int width;
	int height;

	SpellActionBase* action;
	Tooltip* tooltip;

	SpellSlot(int _posX, int _posY, int _width, int _height) {
		posX = _posX;
		posY = _posY;
		width = _width;
		height = _height;
		action = nullptr;
		tooltip = nullptr;
	}
};

class Spellbook : public Widget{

public:

	Spellbook();
	~Spellbook() = default;
	
	void draw();
	void drawSpellTooltip(int mouseX, int mouseY);
	void drawFloatingSpell();
	void processInput();

	void setPlayer(Player* player);
	bool hasFloatingSpell() const;
	void init(unsigned int textureAtlas, std::vector<TextureRect> textures);

	short getMouseOverSpellSlotId(int mouseX, int mouseY) const;
	SpellSlot* getFloatingSpell() const;
	void setFloatingSpell(SpellActionBase* newFloatingSpell);
	void unsetFloatingSpell();
	void reloadSpellsFromPlayer();
	void resize(int deltaW, int deltaH);
	void clear();
	//std::string getLuaSaveText() const;
	
	static Spellbook& Get();

	std::vector<TextureRect> m_textures;
	unsigned int m_textureAtlas;

private:

	bool isMouseOverNextPageArea(int mouseX, int mouseY);
	bool isMouseOverPreviousPageArea(int mouseX, int mouseY);
	void nextPage();
	void previousPage();
	void refreshPage();

	int nextPageButtonOffsetX;
	int nextPageButtonOffsetY;

	int previousPageButtonOffsetX;
	int previousPageButtonOffsetY;

	float pageButtonWidth;
	float pageButtonHeight;

	std::vector<SpellSlot> spellSlot;
	std::vector<SpellActionBase*> inscribedSpells;
	Player* m_player;
	int curPage;

	SpellSlot* m_floatingSpell;
	SpellSlot& m_floatingSpellSlot;

	CharacterSet* m_cooldownFont;

	static Spellbook s_instance;	
};
