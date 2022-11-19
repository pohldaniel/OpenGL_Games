#include <cassert>

#include "Inventory.h"

#include "Item.h"
#include "Character.h"
#include "Player.h"

InventoryItem::InventoryItem(Item* item,
	size_t inventoryPosX,
	size_t inventoryPosY,
	Player* player,
	InventoryItem* copyFrom)
	: item(item),
	player(player),
	currentStackSize(1),
	inventoryPosX(inventoryPosX),
	inventoryPosY(inventoryPosY) {
	// copy properties from the original inventoryItem (if we've suggested so)
	if (copyFrom != NULL) {
		copyAttributes(copyFrom);
	}

	tooltip = new ItemTooltip(item, this, player);
}

InventoryItem::~InventoryItem() {
	delete tooltip;
}

size_t InventoryItem::getInventoryPosX() const {
	return inventoryPosX;
}

size_t InventoryItem::getInventoryPosY() const {
	return inventoryPosY;
}

void InventoryItem::setInventoryPos(size_t inventoryPosX_, size_t inventoryPosY_) {
	inventoryPosX = inventoryPosX_;
	inventoryPosY = inventoryPosY_;
}

size_t InventoryItem::getSizeX() const {
	return item->getSizeX();
}

size_t InventoryItem::getSizeY() const {
	return item->getSizeY();
}

Item* InventoryItem::getItem() const {
	return item;
}

ItemTooltip* InventoryItem::getTooltip() const {
	return tooltip;
}

size_t InventoryItem::getCurrentStackSize() const {
	return currentStackSize;
}

void InventoryItem::setCurrentStackSize(size_t currentStackSize) {
	this->currentStackSize = currentStackSize;
}

void InventoryItem::increaseCurrentStack() {
	if (currentStackSize < getItem()->getMaxStackSize()) {
		currentStackSize++;
		getTooltip()->reloadTooltip();
	}
}

void InventoryItem::decreaseCurrentStack() {
	if (currentStackSize > 0) {
		currentStackSize--;
		getTooltip()->reloadTooltip();
	}
}

bool InventoryItem::isItemStackFull() const {
	if (currentStackSize == getItem()->getMaxStackSize()) {
		return true;
	}

	return false;
}

void InventoryItem::copyAttributes(InventoryItem* copyFrom) {
	currentStackSize = copyFrom->getCurrentStackSize();
}

bool InventoryItem::canPlayerUseItem() const {
	bool useableItem = true;

	if (player->getLevel() < item->getRequiredLevel()) {
		useableItem = false;
	}

	if (item->getArmorType() != ArmorType::NO_ARMOR) {
		useableItem = player->canWearArmorType(item);
	}

	if (item->isUseable()) {

		if (player->getLevel() < item->getSpell()->getRequiredLevel()) {
			useableItem = false;
		}

		if (player->getClass() != item->getSpell()->getRequiredClass() && item->getSpell()->getRequiredClass() != Enums::CharacterClass::ANYCLASS) {
			useableItem = false;
		}

		if (item->getItemType() == ItemType::NEWSPELL) {
			if (player->isSpellInscribedInSpellbook(item->getSpell()) == true) {
				useableItem = false;
			}
		}
	}

	return useableItem;
}
