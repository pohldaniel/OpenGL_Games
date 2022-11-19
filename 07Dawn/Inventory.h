#pragma once

#include "Tooltip.h"

class Player;

class InventoryItem {
public:
	InventoryItem(Item* item, size_t inventoryPosX, size_t inventoryPosY, Player* player, InventoryItem* copyFrom = NULL);
	~InventoryItem();
	size_t getInventoryPosX() const;
	size_t getInventoryPosY() const;
	void setInventoryPos(size_t inventoryPosX, size_t inventoryPosY);
	ItemTooltip* getTooltip() const;

	size_t getSizeX() const;
	size_t getSizeY() const;

	size_t getCurrentStackSize() const;
	void setCurrentStackSize(size_t currentStackSize);
	void increaseCurrentStack();
	void decreaseCurrentStack();
	bool isItemStackFull() const;

	void copyAttributes(InventoryItem *copyFrom);
	Item* getItem() const;
	bool canPlayerUseItem() const;


private:
	Item* item;
	Player* player;
	ItemTooltip* tooltip;

	size_t currentStackSize;

	size_t inventoryPosX;
	size_t inventoryPosY;
};