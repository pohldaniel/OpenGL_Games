#pragma once
#include <cstdarg>

#include "engine/Fontrenderer.h"

#include "ViewPort.h"
#include "SpellActionBase.h"
#include "Spells.h"
#include "Actions.h"
#include "Item.h"
#include "Dialog.h"


class Player;
class InventoryItem;

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
	friend class ItemTooltip;

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
	std::string parseInfoText(SpellActionBase *spell, const std::string infoText) const;
	std::string getDynamicValues(SpellActionBase *spell, size_t val) const;

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
	std::vector<sTooltipText> tooltipTextSmall;

	uint32_t ticketFromPlayer;

	void addTooltipText(Vector4f color, CharacterSet* charSet, std::string str, ...);
};

class SpellTooltip : public Tooltip {

public:
	SpellTooltip(SpellActionBase *parent, Player *player);
	void draw(int x, int y);
	SpellActionBase* getParent() const;

private:
	SpellActionBase *parent;
	void getParentText();
	void getTicketFromPlayer();
};

class ItemTooltip : public Tooltip {
public:
	ItemTooltip(Item *parent, InventoryItem *inventoryItem);
	void draw(int x, int y);
	void setShopItem(bool isShopItem_);
	Item* getParent() const;

	static void DrawCoin(int x, int y, int coin);

private:
	Item *parent;
	InventoryItem *inventoryItem;
	std::string currentCooldownText;

	bool isShopItem;

	std::string itemValue[3];

	void getParentText();
	void getTicketFromPlayer();
	void addTooltipTextForPercentageAttribute(std::string attributeName, double attributePercentage);
	void drawCoinsLine(int x, int y, int frameWidth, sTooltipText *tooltipText);
	

	static std::vector<TextureRect> s_textures;
	static unsigned int s_textureAtlas;

};