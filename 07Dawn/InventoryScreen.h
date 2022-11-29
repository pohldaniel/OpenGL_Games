#pragma once

#include "engine/Fontrenderer.h"

#include "Widget.h"
#include "Player.h"

namespace currency {
	enum currency {
		COPPER, // 100 COPPER == 1 SILVER
		SILVER, // 100 SILVER == 1 GOLD
		GOLD
	};
	std::string getLongTextString(std::uint32_t coins);
	void exchangeCoins(std::uint32_t &copper, std::uint32_t &silver, std::uint32_t &gold, std::uint32_t &coins);
	std::string convertCoinsToString(currency currency, std::uint32_t coins);
}

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

class InventoryScreen : public Widget {

public:
	
	~InventoryScreen();

	void draw() override;
	void processInput() override;

	void drawCoins();
	void drawSlot(Enums::ItemSlot curSlot);
	void drawFloatingSelection();
	void drawItemPlacement(int mouseX, int mouseY);
	void drawItemTooltip(int mouseX, int mouseY);

	void setTextureDependentPositions();

	void init();

	void setPlayer(Player* player);

	bool isOnBackpackScreen(int mouseX, int mouseY) const;
	bool isOverSlot(Enums::ItemSlot itemSlot, int mouseX, int mouseY) const;
	bool hasFloatingSelection() const;

	InventoryItem* getFloatingSelection() const;

	Enums::ItemSlot getMouseOverSlot(int mouseX, int mouseY) const;

	void setFloatingSelection(InventoryItem* item);
	void unsetFloatingSelection();


	static InventoryScreen& Get();

private:

	InventoryScreen();

	Player* m_player;

	CharacterSet* m_coinsFont;
	std::vector<TextureRect> m_textures;
	unsigned int m_textureAtlas;

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
	static InventoryScreen s_instance;
};