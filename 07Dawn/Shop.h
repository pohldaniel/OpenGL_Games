#pragma once
#include <stdint.h>
#include <string>

#include "Widget.h"
#include "CharacterInfo.h"

class InventoryItem;
class Item;
class Npc;
class Shop;

class ShopCanvas : public Widget {

public:

	~ShopCanvas();

	void draw() override;
	void processInput() override;
	void drawItemTooltip(int mouseX, int mouseY);
	void drawFloatingSelection();

	void init(std::vector<TextureRect> textures);
	void setShop(Shop* shop);
	InventoryItem *getFloatingSelection() const;
	bool hasFloatingSelection() const;
	void unsetFloatingSelection();
	Shop* getShop();
	static ShopCanvas& Get();

private:

	ShopCanvas();

	void drawItems();
	void drawTabs();
	
	bool isOnSlotsScreen(int x, int y);
	bool isPositionFree(size_t invPosX, size_t invPosY, size_t curTab) const;
	
	InventoryItem* getItemAt(size_t invPosX, size_t invPosY, size_t itemTab);
	
	CharacterSet* m_itemStackFont;
	std::vector<TextureRect> m_textures;

	unsigned short currentTab;
	Tabs tabs[3];
	size_t backpackFieldWidth;
	size_t backpackFieldHeight;
	size_t backpackSeparatorWidth;
	size_t backpackSeparatorHeight;
	size_t backpackOffsetX;
	size_t backpackOffsetY;

	InventoryItem *floatingSelection;
	Shop* m_shop;

	static ShopCanvas s_instance;
};

class Shop {

	friend class ShopCanvas;
	friend class CZone; // just for now to be able to save the items

public:
	Shop();
	~Shop();

	void loadShopkeeperInventory();
	void addItem(Item *item);
	void setPlayer(Player* player);
	void buyFromShop();

private:
	
	void sellToShop(InventoryItem *sellItem, bool givePlayerMoney);
	unsigned short getItemTab(Item *item);
	bool hasSufficientSpaceAt(size_t inventoryPosX, size_t inventoryPosY, size_t itemSizeX, size_t itemSizeY, size_t itemTab) const;
	void insertItemAt(InventoryItem *newItem, size_t foundX, size_t foundY, size_t itemTab);
	void removeItem(InventoryItem *inventoryItem);

	Player *m_player;
	Npc *m_shopkeeper;

	std::vector<InventoryItem*> shopkeeperInventory[3];

	unsigned short numSlotsX;
	unsigned short numSlotsY;
	bool ***slotUsed;

	ShopCanvas& m_shopCanvas;
};

class ShopManager {

public:

	Shop& getShop(std::string zoneName);
	static ShopManager& Get();

private:
	ShopManager() = default;
	std::unordered_map<std::string, Shop*> m_shops;
	
	static ShopManager s_instance;
};

