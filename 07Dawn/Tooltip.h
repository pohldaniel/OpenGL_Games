#pragma once
#include <cstdarg>

#include "engine/Fontrenderer.h"

#include "ViewPort.h"
#include "SpellActionBase.h"
#include "Spells.h"
#include "Actions.h"
#include "Dialog.h"

class Player;

struct sTooltipText {
	std::string text;
	Vector4f color;
	CharacterSet *charset;

	sTooltipText(std::string text_ , Vector4f _color, CharacterSet* _charset) {
		text = text_;
		color = _color;
		charset = _charset;
	}
};

class Tooltip {

	friend class SpellTooltip;

public:
	~Tooltip();
	void enableSmallTooltip();
	void disableSmallTooltip();
	bool isTooltipSmall();
	virtual void getParentText() = 0;
	virtual void draw(int x, int y) = 0;
	virtual void getTicketFromPlayer() = 0;
	int getTooltipWidth() const;
	int getTooltipHeight() const;
	void reloadTooltip();
	std::string parseInfoText(CSpellActionBase *spell, const std::string infoText) const;
	std::string getDynamicValues(CSpellActionBase *spell, size_t val) const;

private:
	/** \brief adjusts curBlockNumberWidth and curBlockNumberHeight according to current tooltip size */
	void updateBlockNumbers();

	bool smallTooltip; // with this true, tooltip will merely display the name of things.
	int toplineHeight;
	int blockWidth;
	int blockHeight;
	int blockNumberWidth;
	int blockNumberHeight;
	int blockNumberWidthSmall;
	int blockNumberHeightSmall;
	int curBlockNumberWidth;
	int curBlockNumberHeight;
	bool shoppingState;
	Player *player;
	std::vector<sTooltipText> tooltipText;

	uint32_t ticketFromPlayer;

	void loadTextures();
	void addTooltipText(Vector4f color, CharacterSet* fontSize, std::string str, ...);
};

class SpellTooltip : public Tooltip {

public:
	SpellTooltip(CSpellActionBase *parent, Player *player);
	void draw(int x, int y);
	CSpellActionBase *getParent() const;

private:
	CSpellActionBase *parent;
	void getParentText();
	void getTicketFromPlayer();
};

namespace DrawFunctions {
	void initDrawTextures();
	void drawCoin(int x, int y, int coin);
}