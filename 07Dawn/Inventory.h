#pragma once
#include <stddef.h>
#include <vector>
#include "Enums.h"

class Player;
class ItemTooltip;
class Item;

class InventoryItem {
public:
	InventoryItem(Item* item, size_t inventoryPosX, size_t inventoryPosY, InventoryItem* copyFrom = NULL);
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
	bool canPlayerUseItem(Player& player) const;


private:
	Item* item;
	//Player* player;
	ItemTooltip* tooltip;

	size_t currentStackSize;

	size_t inventoryPosX;
	size_t inventoryPosY;
};

class Inventory {

public:
	Inventory(size_t sizeX_, size_t sizeY_, Player* player_);
	~Inventory();

	bool insertItem(Item* item, InventoryItem* oldInventoryItem = NULL);
	bool hasSufficientSpaceAt(size_t invPosX, size_t invPosY, size_t itemSizeX, size_t itemSizeY) const;
	bool hasSufficientSpaceWithExchangeAt(size_t inventoryPosX, size_t inventoryPosY, size_t itemSizeX, size_t itemSizeY);
	bool stackItemIfPossible(Item* item, InventoryItem* itemToStack) const;

	std::vector<InventoryItem*> getEquippedItems() const;
	std::vector<InventoryItem*> getBackpackItems() const;

	void wieldItemAtSlot(Enums::ItemSlot slotToUse, InventoryItem *item);
	InventoryItem* getItemAtSlot(Enums::ItemSlot slotToUse);

	bool isPositionFree(size_t invPosX, size_t invPosY) const;
	InventoryItem* getItemAt(size_t invPosX, size_t invPosY);
	std::vector<InventoryItem*> getIdenticalItemsFromBackpack(Item* item) const;
	bool doesItemExistInBackpack(Item* item, int quantity) const;

	bool isWieldingTwoHandedWeapon() const;
	Enums::WeaponType getWeaponTypeBySlot(Enums::ItemSlot itemSlot) const;

	InventoryItem* insertItemWithExchangeAt(InventoryItem* inventoryItem, size_t invPosX, size_t invPosY);
	bool containsItem(InventoryItem* inventoryItem) const;
	void removeItem(InventoryItem* inventoryItem);
	void removeItem(Item* item);
	std::string getReloadText();

	void clear();

	static EquipPosition::EquipPosition getEquipType(Enums::ItemSlot itemSlot);

private:
	void insertItemAt(InventoryItem* inventoryItem, size_t invPosX, size_t invPosY);
	InventoryItem* findFirstBlockingItem(size_t inventoryPosX, size_t inventoryPosY, size_t itemSizeX, size_t itemSizeY);

	size_t sizeX;
	size_t sizeY;
	bool** slotUsed;
	Player* player;
	std::vector<InventoryItem*> backpackItems;
	InventoryItem** equippedItems;
};