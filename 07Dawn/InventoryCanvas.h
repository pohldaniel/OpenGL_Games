#pragma once

#include "engine/Fontrenderer.h"

#include "Widget.h"
#include "Player.h"


class InventoryItem;

class InventoryScreenSlot {
private:
	Enums::ItemSlot itemSlot;
	size_t offsetX;
	size_t offsetY;
	size_t sizeX;
	size_t sizeY;
	TextureRect texture;

public:
	InventoryScreenSlot(Enums::ItemSlot itemSlot, size_t offsetX, size_t offsetY, size_t sizeX, size_t sizeY, TextureRect texture);
	size_t getOffsetX() const;
	size_t getOffsetY() const;
	size_t getSizeX() const;
	size_t getSizeY() const;
	TextureRect *getTexture();

	Enums::ItemSlot getItemSlot() const;
};

class InventoryCanvas : public Widget {

public:

	~InventoryCanvas();

	void draw() override;
	void processInput() override;

	void drawCoins();
	void drawSlot(Enums::ItemSlot curSlot);
	void drawFloatingSelection();
	void drawItemPlacement(int mouseX, int mouseY);
	void drawItemTooltip(int mouseX, int mouseY);

	void setTextureDependentPositions();

	void init(std::vector<TextureRect> textures);

	void setPlayer(Player* player);

	bool isOnBackpackScreen(int mouseX, int mouseY) const;
	bool isOverSlot(Enums::ItemSlot itemSlot, int mouseX, int mouseY) const;
	bool hasFloatingSelection() const;

	InventoryItem* getFloatingSelection() const;

	Enums::ItemSlot getMouseOverSlot(int mouseX, int mouseY) const;

	void setFloatingSelection(InventoryItem* item);
	void unsetFloatingSelection();

	static InventoryCanvas& Get();

private:

	InventoryCanvas();

	Player* m_player;

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
	void addInventoryScreenSlot(InventoryScreenSlot** mySlots, Enums::ItemSlot slotToUse, size_t offsetX, size_t offsetY, size_t sizeX, size_t sizeY, TextureRect _texture);
	static InventoryCanvas s_instance;
};