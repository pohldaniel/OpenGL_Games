#pragma once

#include "Item.h"
#include "Inventory.h"

class Player;

struct sGroundItems {
	int xpos, ypos, tooltipXpos, tooltipYpos, tooltipWidth;
	std::string itemValue[3];
	int coinsOffset[3];
	int coinsTextOffset[3];
	GLfloat color[4];
	Item *item;
	void loadTooltip();

	sGroundItems(int xpos_, int ypos_, Item *item_, int fontLength) {
		xpos = xpos_;
		ypos = ypos_;
		item = item_;
		tooltipWidth = fontLength;
		loadTooltip();
	}

	bool operator<(const sGroundItems& item1) const { // instead of using a predicate in our sort call.
		return tooltipYpos < item1.tooltipYpos;
	};
};

class GroundLoot {
	friend class Zone;

public:
	GroundLoot(Player *player_);
	~GroundLoot();

	InventoryItem *getFloatingSelection(int x, int y);
	bool pickUpLoot(Player *player_, sGroundItems groundItem, size_t curItem);
	void searchForItems(int mouseX, int mouseY);
	void lootItem(Item *item, size_t itemIndex);
	void addItem(int x, int y, Item *newItem);
	void removeItem(size_t pos);
	void removeItem(Item *item);
	void sortItems();
	void draw();
	void drawTooltip(int mouseX, int mouseY);
	void enableTooltips();
	void disableTooltips();

	static void DrawTooltip(int mouseX, int mouseY);
	static GroundLoot& GetGroundLoot();
	static void ProcessInput(int mouseX, int mouseY);

private:
	std::vector<sGroundItems> groundItems;

	std::vector<TextureRect> m_textures;
	unsigned int m_textureAtlas;
	Player *player;
	bool drawTooltips;
	CharacterSet* m_font;
};