#pragma once
#include "Spells.h"
#include "Actions.h"
#include "Tooltip.h"
#include "Widget.h"

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

class Spellbook : public Widget {

public:

	~Spellbook() = default;
	
	void draw() override;
	void drawSpellTooltip(int mouseX, int mouseY);
	void drawFloatingSpell();
	void processInput() override;

	void setPlayer(Player* player);
	bool hasFloatingSpell() const;
	void init(std::vector<TextureRect> textures);

	short getMouseOverSpellSlotId(int mouseX, int mouseY) const;
	SpellSlot* getFloatingSpell() const;
	void setFloatingSpell(SpellActionBase* newFloatingSpell);
	void unsetFloatingSpell();
	void reloadSpellsFromPlayer();
	void clear();
	std::string getLuaSaveText() const;
	
	static Spellbook& Get();

private:

	Spellbook();

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
	std::vector<TextureRect> m_textures;

	static Spellbook s_instance;	
};
