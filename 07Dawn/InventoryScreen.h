#pragma once

#include "engine/Rect.h"
#include "Enums.h"
#include "Widget.h"

class Player;
class InventoryItem;

class InventoryScreenSlot {
private:
	Enums::ItemSlot itemSlot;
	size_t offsetX;
	size_t offsetY;
	size_t sizeX;
	size_t sizeY;
	TextureRect textures;

public:
	InventoryScreenSlot(Enums::ItemSlot itemSlot, size_t offsetX, size_t offsetY, size_t sizeX, size_t sizeY, std::string plain_file);
	size_t getOffsetX() const;
	size_t getOffsetY() const;
	size_t getSizeX() const;
	size_t getSizeY() const;
	TextureRect *getTexture();

	Enums::ItemSlot getItemSlot() const;
};

class InventoryScreen : public Widget {
private:
	Player* player;

	CharacterSet* m_coinsFont;
	std::vector<TextureRect> m_textures;


	InventoryItem* floatingSelection;
	InventoryScreenSlot** mySlots;



	void drawBackpack();
	void dropItemOnGround(InventoryItem* item);

	size_t backpackFieldWidth;
	size_t backpackFieldHeight;
	size_t backpackSeparatorWidth;
	size_t backpackSeparatorHeight;
	size_t backpackOffsetX;
	size_t backpackOffsetY;
	size_t numSlotsX;
	size_t numSlotsY;

	void equipOnSlotOriginDependingAndPlaySound(Enums::ItemSlot slotToUse, InventoryItem* wieldItem, bool fromShop, InventoryItem* newFloatingSelection);

public:
	InventoryScreen(Player* player_);
	~InventoryScreen();

	void clicked(int clickX, int clickY, uint8_t mouseState);

	void loadTextures();
	void setTextureDependentPositions();

	void draw(int mouseX, int mouseY);
	void drawCoins();
	void drawSlot(Enums::ItemSlot curSlot);
	void drawFloatingSelection(int mouseX, int mouseY);
	void drawItemPlacement(int mouseX, int mouseY);
	void drawItemTooltip(int mouseX, int mouseY);

	bool isMouseOnThisFrame(int mouseX, int mouseY) const;
	bool isOnBackpackScreen(int mouseX, int mouseY) const;
	bool isOverSlot(Enums::ItemSlot itemSlot, int mouseX, int mouseY) const;
	bool hasFloatingSelection() const;

	InventoryItem* getFloatingSelection() const;

	Enums::ItemSlot getMouseOverSlot(int mouseX, int mouseY) const;

	void setFloatingSelection(InventoryItem* item);
	void unsetFloatingSelection();
};