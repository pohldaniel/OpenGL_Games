#include "InventoryCanvas.h"
#include "Inventory.h"
#include "Luainterface.h"
#include "Spells.h"
#include "Shop.h"
#include "Zone.h"
#include "Utils.h"

InventoryCanvas InventoryCanvas::s_instance;

InventoryCanvas& InventoryCanvas::Get() {
	return s_instance;
}

InventoryScreenSlot::InventoryScreenSlot(Enums::ItemSlot _itemSlot, size_t _offsetX, size_t _offsetY, size_t _sizeX, size_t _sizeY, TextureRect _texture)
	: itemSlot(_itemSlot),
	offsetX(_offsetX),
	offsetY(_offsetY),
	sizeX(_sizeX),
	sizeY(_sizeY),
	texture(_texture) {

}

size_t InventoryScreenSlot::getOffsetX() const {
	return offsetX;
}

size_t InventoryScreenSlot::getOffsetY() const {
	return offsetY;
}

size_t InventoryScreenSlot::getSizeX() const {
	return sizeX;
}

size_t InventoryScreenSlot::getSizeY() const {
	return sizeY;
}

Enums::ItemSlot InventoryScreenSlot::getItemSlot() const {
	return itemSlot;
}

TextureRect* InventoryScreenSlot::getTexture() {
	return &texture;
}

InventoryCanvas::InventoryCanvas() : Widget(0, 80, 469, 654, 15, 17) {
	mySlots = new InventoryScreenSlot*[static_cast<size_t>(Enums::ItemSlot::COUNTIS)];
	for (size_t curSlotNr = 0; curSlotNr < static_cast<size_t>(Enums::ItemSlot::COUNTIS); ++curSlotNr) {
		mySlots[curSlotNr] = NULL;
	}
}

InventoryCanvas::~InventoryCanvas() {

	size_t count = static_cast<size_t>(Enums::ItemSlot::COUNTIS);
	for (size_t curSlot = 0; curSlot < count; ++curSlot) {
		delete mySlots[curSlot];
	}
	delete[] mySlots;

}

void InventoryCanvas::setPlayer(Player* player) {
	m_player = player;
}

void InventoryCanvas::init() {

	TextureAtlasCreator::get().init("inventoryscreen", 1024, 1024);
	TextureManager::Loadimage("res/interface/inventory/base.tga", 0, m_textures);
	TextureManager::Loadimage("res/white2x2pixel.png", 1, m_textures, true);
	TextureManager::Loadimage("res/interface/inventory/goldcoin.tga", 2, m_textures, true);
	TextureManager::Loadimage("res/interface/inventory/silvercoin.tga", 3, m_textures, true);
	TextureManager::Loadimage("res/interface/inventory/coppercoin.tga", 4, m_textures, true);


	TextureManager::Loadimage("res/interface/inventory/head.tga", 5, m_textures);
	TextureManager::Loadimage("res/interface/inventory/amulet.tga", 6, m_textures);
	TextureManager::Loadimage("res/interface/inventory/main_hand.tga", 7, m_textures);
	TextureManager::Loadimage("res/interface/inventory/chest.tga", 8, m_textures);
	TextureManager::Loadimage("res/interface/inventory/belt.tga", 9, m_textures);


	TextureManager::Loadimage("res/interface/inventory/legs.tga", 10, m_textures);
	TextureManager::Loadimage("res/interface/inventory/shoulder.tga", 11, m_textures);
	TextureManager::Loadimage("res/interface/inventory/cloak.tga", 12, m_textures);
	TextureManager::Loadimage("res/interface/inventory/gloves.tga", 13, m_textures);
	TextureManager::Loadimage("res/interface/inventory/off_hand.tga", 14, m_textures);

	TextureManager::Loadimage("res/interface/inventory/ring_one.tga", 15, m_textures);
	TextureManager::Loadimage("res/interface/inventory/ring_two.tga", 16, m_textures);
	TextureManager::Loadimage("res/interface/inventory/boots.tga", 17, m_textures);

	m_textureAtlas = TextureAtlasCreator::get().getAtlas();
	m_textureAtlas = Spritesheet::Merge(TextureManager::GetTextureAtlas("items"), m_textureAtlas, false, true);
	for (unsigned short layer = 0; layer < m_textures.size(); layer++) {
		m_textures[layer].frame++;
	}

	//Spritesheet::Safe("interface", m_textureAtlas);	
	floatingSelection = NULL;
	backpackFieldWidth = 32;
	backpackFieldHeight = 32;
	backpackSeparatorWidth = 3;
	backpackSeparatorHeight = 3;
	backpackOffsetX = 69;
	backpackOffsetY = 59;
	numSlotsX = 10;
	numSlotsY = 4;

	addInventoryScreenSlot(mySlots, Enums::ItemSlot::HEAD, 210, 556, 64, 64, m_textures[5]);
	addInventoryScreenSlot(mySlots, Enums::ItemSlot::AMULET, 171, 532, 32, 32, m_textures[6]);
	addInventoryScreenSlot(mySlots, Enums::ItemSlot::MAIN_HAND, 97, 412, 64, 96, m_textures[7]);
	addInventoryScreenSlot(mySlots, Enums::ItemSlot::CHEST, 210, 450, 64, 96, m_textures[8]);
	addInventoryScreenSlot(mySlots, Enums::ItemSlot::BELT, 210, 408, 64, 32, m_textures[9]);
	addInventoryScreenSlot(mySlots, Enums::ItemSlot::LEGS, 210, 302, 64, 96, m_textures[10]);
	addInventoryScreenSlot(mySlots, Enums::ItemSlot::SHOULDER, 281, 493, 64, 64, m_textures[11]);
	addInventoryScreenSlot(mySlots, Enums::ItemSlot::CLOAK, 294, 284, 64, 64, m_textures[12]);
	addInventoryScreenSlot(mySlots, Enums::ItemSlot::GLOVES, 284, 376, 64, 64, m_textures[13]);
	addInventoryScreenSlot(mySlots, Enums::ItemSlot::OFF_HAND, 354, 412, 64, 96, m_textures[14]);
	addInventoryScreenSlot(mySlots, Enums::ItemSlot::RING_ONE, 117, 362, 32, 32, m_textures[15]);
	addInventoryScreenSlot(mySlots, Enums::ItemSlot::RING_TWO, 159, 362, 32, 32, m_textures[16]);
	addInventoryScreenSlot(mySlots, Enums::ItemSlot::BOOTS, 210, 229, 64, 64, m_textures[17]);

	// load the font for coin text.
	m_coinsFont = &Globals::fontManager.get("verdana_12");

	addMoveableFrame(454, 21, 15, 650);
	addCloseButton(22, 22, 447, 650);
	setTextureDependentPositions();
}

void InventoryCanvas::draw() {
	if (!m_visible) return;
	TextureManager::BindTexture(m_textureAtlas, true, 0);

	TextureManager::DrawTextureBatched(m_textures[0], m_posX, m_posY, false, false);

	drawCoins();
	drawBackpack();
	for (size_t curSlotNr = 0; curSlotNr < static_cast<size_t>(Enums::ItemSlot::COUNTIS); ++curSlotNr) {
		drawSlot(static_cast<Enums::ItemSlot>(curSlotNr));
	}
	drawItemPlacement(ViewPort::get().getCursorPosRelX(), ViewPort::get().getCursorPosRelY());


	TextureManager::DrawBuffer();
}

void InventoryCanvas::drawCoins() {
	// draws our coins in gold, silver and copper.
	std::string gold = currency::convertCoinsToString(currency::GOLD, m_player->getCoins());
	std::string silver = currency::convertCoinsToString(currency::SILVER, m_player->getCoins());
	std::string copper = currency::convertCoinsToString(currency::COPPER, m_player->getCoins());

	// gold coin
	TextureManager::DrawTextureBatched(m_textures[2], m_posX + 167, m_posY + 308, false, false);
	// silver coin
	TextureManager::DrawTextureBatched(m_textures[3], m_posX + 167, m_posY + 286, false, false);
	// copper coin
	TextureManager::DrawTextureBatched(m_textures[4], m_posX + 167, m_posY + 264, false, false);

	Fontrenderer::Get().addText(*m_coinsFont, m_posX + 160 - m_coinsFont->getWidth(gold), m_posY + 307, gold, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
	Fontrenderer::Get().addText(*m_coinsFont, m_posX + 160 - m_coinsFont->getWidth(silver), m_posY + 285, silver, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
	Fontrenderer::Get().addText(*m_coinsFont, m_posX + 160 - m_coinsFont->getWidth(copper), m_posY + 263, copper, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
}

void InventoryCanvas::drawBackpack() {
	Inventory* inventory = m_player->getInventory();
	std::vector<InventoryItem*> items = inventory->getBackpackItems();
	size_t numItems = items.size();

	for (size_t curItemNr = 0; curItemNr<numItems; ++curItemNr) {

		InventoryItem* curInvItem = items[curItemNr];
		Item* curItem = curInvItem->getItem();
		TextureRect* symbolTexture = curItem->getSymbolTexture();

		size_t invPosX = curInvItem->getInventoryPosX();
		size_t invPosY = curInvItem->getInventoryPosY();
		size_t sizeX = curItem->getSizeX();
		size_t sizeY = curItem->getSizeY();

		TextureManager::DrawTextureBatched(*symbolTexture, m_posX + backpackOffsetX + invPosX * backpackFieldWidth + invPosX * backpackSeparatorWidth, m_posY + backpackOffsetY + invPosY * backpackFieldHeight + invPosY * backpackSeparatorHeight, backpackFieldWidth * sizeX + (sizeX - 1)*backpackSeparatorWidth, backpackFieldHeight * sizeY + (sizeY - 1)*backpackSeparatorHeight, false, false);
		// if we have an item that is stackable, and the stacksize is more than 1, we draw that number.
		if (curInvItem->getCurrentStackSize() > 1) {
			Fontrenderer::Get().addText(*m_coinsFont, m_posX + backpackOffsetX + backpackFieldWidth - m_coinsFont->getWidth(std::to_string(curInvItem->getCurrentStackSize())) + invPosX * backpackFieldWidth + invPosX * backpackSeparatorWidth, m_posY + backpackOffsetY + invPosY * backpackFieldHeight + invPosY * backpackSeparatorHeight, std::to_string(curInvItem->getCurrentStackSize()), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
		}
	}
}

void InventoryCanvas::drawSlot(Enums::ItemSlot curSlot) {
	Inventory* inventory = m_player->getInventory();
	InventoryItem* invItem = inventory->getItemAtSlot(curSlot);
	if (invItem != NULL) {
		Item* item = invItem->getItem();
		TextureRect* symbolTexture = item->getSymbolTexture();

		InventoryScreenSlot* curScreenSlot = mySlots[static_cast<size_t>(curSlot)];

		size_t drawSizeX = symbolTexture->width;
		size_t drawSizeY = symbolTexture->height;

		size_t centerOffsetX = (curScreenSlot->getSizeX() - drawSizeX) / 2;
		size_t centerOffsetY = (curScreenSlot->getSizeY() - drawSizeY) / 2;
		//TextureManager::BindTexture(m_textureAtlas, true);
		// draw the plain background image of the item, hiding the item placeholder.
		TextureManager::DrawTextureBatched(*curScreenSlot->getTexture(), m_posX + curScreenSlot->getOffsetX(), m_posY + curScreenSlot->getOffsetY(), curScreenSlot->getTexture()->width, curScreenSlot->getTexture()->height, false, false);

		// we draw the two-handed weapons on our off-handslot with 50% transparency
		Vector4f color = item->isTwoHandedWeapon() == true && curSlot == Enums::ItemSlot::OFF_HAND ? Vector4f(1.0f, 1.0f, 1.0f, 0.5f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		TextureManager::DrawTextureBatched(*symbolTexture, m_posX + curScreenSlot->getOffsetX() + centerOffsetX, m_posY + curScreenSlot->getOffsetY() + centerOffsetY, drawSizeX, drawSizeY, color, false, false);
	}
}

void InventoryCanvas::drawItemPlacement(int mouseX, int mouseY) {
	if (!m_visible) return;

	bool floatingSelectionFromShop = false;

	InventoryItem* floatingSelectionToDraw = floatingSelection;
	if (floatingSelectionToDraw == NULL && ShopCanvas::Get().hasFloatingSelection()) {
		floatingSelectionToDraw = ShopCanvas::Get().getFloatingSelection();
		floatingSelectionFromShop = true;
	}

	if (floatingSelectionToDraw == NULL) return;

	if (isOnBackpackScreen(mouseX, mouseY)) {
		Item* floatingItem = floatingSelectionToDraw->getItem();
		Inventory* inventory = m_player->getInventory();
		GLfloat shade[4] = { 0.0f, 0.0f, 0.0f, 0.3f };
		size_t sizeX = floatingItem->getSizeX();
		size_t sizeY = floatingItem->getSizeY();

		// calculate which backpack-slot we are looking at.
		int fieldIndexX = (mouseX - (m_posX + backpackOffsetX)) / (backpackFieldWidth + backpackSeparatorWidth);
		int fieldIndexY = (mouseY - (m_posY + backpackOffsetY)) / (backpackFieldHeight + backpackSeparatorHeight);

		// set the shade-color depending on if the item fits or not.
		if (inventory->hasSufficientSpaceWithExchangeAt(fieldIndexX, fieldIndexY, sizeX, sizeY)) {
			if (floatingSelectionFromShop) {
				// yellow color (item needs to be paid)
				shade[0] = 1.0f;
				shade[1] = 1.0f;
			}
			else {
				shade[1] = 1.0f; // green color
			}
		}
		else {
			shade[0] = 1.0f; // red color
		}

		// calculate the size of the shade, if too big, we resize it.
		int shadePosX = m_posX + backpackOffsetX
			+ fieldIndexX * backpackFieldWidth
			+ fieldIndexX * backpackSeparatorWidth;
		int shadePosY = m_posY + backpackOffsetY - 1
			+ fieldIndexY * backpackFieldHeight
			+ fieldIndexY * backpackSeparatorHeight;
		int shadeWidth = backpackFieldWidth * sizeX + (sizeX - 1)*backpackSeparatorWidth;
		int shadeHeight = backpackFieldHeight * sizeY + (sizeY - 1)*backpackSeparatorHeight;

		if (sizeY + fieldIndexY > numSlotsY) {
			shadeHeight = backpackFieldWidth * (sizeY - ((sizeY + fieldIndexY) - numSlotsY)) +
				(sizeY - ((sizeY + fieldIndexY) - (numSlotsY - 1)))*backpackSeparatorWidth;
		}

		if (sizeX + fieldIndexX > numSlotsX) {
			shadeWidth = backpackFieldHeight * (sizeX - ((sizeX + fieldIndexX) - numSlotsX)) +
				(sizeX - ((sizeX + fieldIndexX) - (numSlotsX - 1)))*backpackSeparatorHeight;
		}

		TextureManager::DrawTextureBatched(m_textures[1], shadePosX, shadePosY, shadeWidth, shadeHeight, Vector4f(shade[0], shade[1], shade[2], shade[3]), false, false);

		return;
	}

	for (size_t curSlotNr = 0; curSlotNr<static_cast<size_t>(Enums::ItemSlot::COUNTIS); ++curSlotNr) {
		Enums::ItemSlot curSlotEnum = static_cast<Enums::ItemSlot>(curSlotNr);
		if (isOverSlot(curSlotEnum, mouseX, mouseY)) {
			GLfloat shade[4] = { 0.0f, 0.0f, 0.0f, 0.3f };

			// set the shade-color depending on if the item fits or not.
			if (floatingSelectionToDraw->canPlayerUseItem(*m_player) == true &&
				floatingSelectionToDraw->getItem()->getEquipPosition() == Inventory::getEquipType(curSlotEnum)) {
				if (floatingSelectionFromShop) {
					// yellow color (item needs to be paid)
					shade[0] = 1.0f;
					shade[1] = 1.0f;
				}
				else {
					shade[1] = 1.0f; // green color
				}
			}
			else {
				shade[0] = 1.0f; // red color
			}

			InventoryScreenSlot* curScreenSlot = mySlots[curSlotNr];
			TextureManager::DrawTextureBatched(m_textures[1], m_posX + curScreenSlot->getOffsetX(), m_posY + curScreenSlot->getOffsetY(), curScreenSlot->getSizeX(), curScreenSlot->getSizeY(), Vector4f(shade[0], shade[1], shade[2], shade[3]), false, false);
			return;
		}
	}
}

void InventoryCanvas::drawFloatingSelection() {
	// draw floating selection
	if (floatingSelection != NULL) {
		Item* floatingItem = floatingSelection->getItem();
		size_t sizeX = floatingItem->getSizeX();
		size_t sizeY = floatingItem->getSizeY();
		TextureManager::BindTexture(TextureManager::GetTextureAtlas("items"), true);
		TextureManager::DrawTexture(*floatingItem->getSymbolTexture(), ViewPort::get().getCursorPosRelX(), ViewPort::get().getCursorPosRelY() - 20, backpackFieldWidth * sizeX + (sizeX - 1) * backpackSeparatorWidth, backpackFieldHeight * sizeY + (sizeY - 1) * backpackSeparatorHeight, false, false);
		TextureManager::UnbindTexture(true);
	}
}

void InventoryCanvas::drawItemTooltip(int mouseX, int mouseY) {
	if (!m_visible) return;
	// draws tooltip over item in the backpack
	InventoryItem* floatingSelectionToHandle = floatingSelection;
	if (floatingSelectionToHandle == NULL && ShopCanvas::Get().hasFloatingSelection()) {
		floatingSelectionToHandle = ShopCanvas::Get().getFloatingSelection();
	}

	if (isOnBackpackScreen(mouseX, mouseY) && isVisible() && floatingSelectionToHandle == NULL) {
		Inventory* inventory = m_player->getInventory();
		InventoryItem* tooltipItem;
		int fieldIndexX = (mouseX - (m_posX + backpackOffsetX)) / (backpackFieldWidth + backpackSeparatorWidth);
		int fieldIndexY = (mouseY - (m_posY + backpackOffsetY)) / (backpackFieldHeight + backpackSeparatorHeight);

		if (!inventory->isPositionFree(fieldIndexX, fieldIndexY)) {
			tooltipItem = inventory->getItemAt(fieldIndexX, fieldIndexY);
			tooltipItem->getTooltip()->setShopItem(false);
			tooltipItem->getTooltip()->draw(mouseX, mouseY);

			//if player is holding down right shift and has an
			// item with same slot equipped, we draw that too.			
			int thisTooltipPosX;
			int previousFrameHeight;
			bool firstItemCompared = false;

			Keyboard &keyboard = Keyboard::instance();
			if (keyboard.keyDown(Keyboard::KEY_LSHIFT)) {
				std::vector<InventoryItem*> equippedItems = inventory->getEquippedItems();
				for (size_t curItem = 0; curItem < equippedItems.size(); curItem++) {
					if (equippedItems[curItem]->getItem()->getEquipPosition() == tooltipItem->getItem()->getEquipPosition()) {
						int thisTooltipPosY = mouseY;
						// if this is our second item we're adding to the compare, then we need to position it next to the previous tooltip.
						if (firstItemCompared == true) {
							thisTooltipPosY += previousFrameHeight + 30;
						}

						// if this is the first (or only) item we're going to draw in the compare we check where it will fit.
						if (firstItemCompared == false) {
							if (ViewPort::get().getWidth() - (mouseX + tooltipItem->getTooltip()->getTooltipWidth() + 60) > equippedItems[curItem]->getTooltip()->getTooltipWidth()) {
								thisTooltipPosX = mouseX + tooltipItem->getTooltip()->getTooltipWidth() + 30;
							}
							else {
								thisTooltipPosX = mouseX - 30 - equippedItems[curItem]->getTooltip()->getTooltipWidth();
							}
						}

						previousFrameHeight = equippedItems[curItem]->getTooltip()->getTooltipHeight();
						equippedItems[curItem]->getTooltip()->draw(thisTooltipPosX, thisTooltipPosY);
						firstItemCompared = true;
					}
				}
			}
		}
	}

	// draws tooltip over equipped item
	if (isVisible() && floatingSelectionToHandle == NULL && !isOnBackpackScreen(mouseX, mouseY)) {
		Enums::ItemSlot tooltipslot = getMouseOverSlot(mouseX, mouseY);

		if (tooltipslot != Enums::ItemSlot::COUNTIS) {
			Inventory* inventory = m_player->getInventory();
			InventoryItem* tooltipItem;

			tooltipItem = inventory->getItemAtSlot(tooltipslot);
			if (tooltipItem) {
				tooltipItem->getTooltip()->setShopItem(false);
				tooltipItem->getTooltip()->draw(mouseX, mouseY);
			}
		}
	}
}

void InventoryCanvas::processInput() {
	if (!m_visible && !hasFloatingSelection()) return;
	Widget::processInput();

	Mouse &mouse = Mouse::instance();
	if (mouse.buttonPressed(Mouse::BUTTON_LEFT) || mouse.buttonPressed(Mouse::BUTTON_RIGHT)) {

		// Put Floating selection out of inventory.
		// Check several positions here for equipping
		Inventory* inventory = m_player->getInventory();

		InventoryItem* floatingSelectionToHandle = floatingSelection;
		bool shopFloatingSelection = false;
		if (floatingSelectionToHandle == NULL && ShopCanvas::Get().hasFloatingSelection()) {
			floatingSelectionToHandle = ShopCanvas::Get().getFloatingSelection();
			shopFloatingSelection = true;
		}
		if (mouse.buttonPressed(Mouse::BUTTON_LEFT)) {
			if (!isMouseOnFrame(mouse.xPosAbsolute(), mouse.yPosAbsolute()) && !ShopCanvas::Get().isMouseOnFrame(mouse.xPosAbsolute(), mouse.yPosAbsolute())) {

				// clicked outside inventory window
				if (floatingSelection != NULL) {
					// drop item...
					dropItemOnGround(floatingSelection);
					if (inventory->containsItem(floatingSelection)) {
						inventory->removeItem(floatingSelection);
					}

					unsetFloatingSelection();

					return;
				}
			}

			for (size_t curSlotNr = 0; curSlotNr < static_cast<size_t>(Enums::ItemSlot::COUNTIS); ++curSlotNr) {
				Enums::ItemSlot curSlotEnum = static_cast<Enums::ItemSlot>(curSlotNr);
				if (isOverSlot(curSlotEnum, ViewPort::get().getCursorPosRelX(), ViewPort::get().getCursorPosRelY())) {
					if (floatingSelectionToHandle != NULL && floatingSelectionToHandle->canPlayerUseItem(*m_player) == true) {
						if (floatingSelectionToHandle->getItem()->getEquipPosition() == Inventory::getEquipType(curSlotEnum)) {
							// special handler for when we are trying to wield a two-handed weapon and having items in BOTH mainhand and offhand-slot equipped.
							if (floatingSelectionToHandle->getItem()->isTwoHandedWeapon() == true && inventory->isWieldingTwoHandedWeapon() == false && inventory->getItemAtSlot(Enums::ItemSlot::MAIN_HAND) != NULL && inventory->getItemAtSlot(Enums::ItemSlot::OFF_HAND) != NULL) {
								if (inventory->insertItem(inventory->getItemAtSlot(Enums::ItemSlot::OFF_HAND)->getItem()) == true) {
									// successfully put the offhand in the inventory. now we just swap the floatingselection with the equipped item.
									equipOnSlotOriginDependingAndPlaySound(curSlotEnum, floatingSelectionToHandle, shopFloatingSelection, inventory->getItemAtSlot(curSlotEnum));
								}

							}
							else if (floatingSelectionToHandle->getItem()->isTwoHandedWeapon() == true && inventory->isWieldingTwoHandedWeapon() == false && inventory->getItemAtSlot(Enums::ItemSlot::OFF_HAND) != NULL) {
								// special handler for when we are trying to wield a two-handed weapon and having ONLY an item in offhand-slot equipped.
								equipOnSlotOriginDependingAndPlaySound(curSlotEnum, floatingSelectionToHandle, shopFloatingSelection, inventory->getItemAtSlot(Enums::ItemSlot::OFF_HAND));

							}
							else if (inventory->getItemAtSlot(curSlotEnum) == NULL) {
								equipOnSlotOriginDependingAndPlaySound(curSlotEnum, floatingSelectionToHandle, shopFloatingSelection, NULL);
							}
							else {

								equipOnSlotOriginDependingAndPlaySound(curSlotEnum, floatingSelectionToHandle, shopFloatingSelection, inventory->getItemAtSlot(curSlotEnum));
							}
						}
					}
					else if (floatingSelectionToHandle == NULL && inventory->getItemAtSlot(curSlotEnum) != NULL) {
						equipOnSlotOriginDependingAndPlaySound(curSlotEnum, NULL, false, inventory->getItemAtSlot(curSlotEnum));
					}
					return;
				}
			}

			if (!isOnBackpackScreen(ViewPort::get().getCursorPosRelX(), ViewPort::get().getCursorPosRelY())) {
				return;
			}
		}
		// calculate field index under mouse
		int fieldIndexX = (ViewPort::get().getCursorPosRelX() - (m_posX + backpackOffsetX)) / (backpackFieldWidth + backpackSeparatorWidth);
		int fieldIndexY = (ViewPort::get().getCursorPosRelY() - (m_posY + backpackOffsetY)) / (backpackFieldHeight + backpackSeparatorHeight);

		if (mouse.buttonPressed(Mouse::BUTTON_RIGHT)) {
			if (!inventory->isPositionFree(fieldIndexX, fieldIndexY)) {
				InventoryItem* useItem = inventory->getItemAt(fieldIndexX, fieldIndexY);

				if (useItem->getItem()->isUseable() &&
					useItem->getItem()->getRequiredLevel() <= m_player->getLevel() &&
					useItem->getItem()->getSpell()->getRequiredLevel() <= m_player->getLevel() &&
					(useItem->getItem()->getSpell()->getRequiredClass() == m_player->getClass() || useItem->getItem()->getSpell()->getRequiredClass() == Enums::CharacterClass::ANYCLASS) &&
					!m_player->getIsPreparing() &&
					m_player->isSpellOnCooldown(useItem->getItem()->getSpell()->getName()) == false) {
					if (useItem->getItem()->getItemType() == Enums::ItemType::NEWSPELL) {
						if (m_player->isSpellInscribedInSpellbook(useItem->getItem()->getSpell()) == true) {
							// spell is already inscribed, return without learning it again.
							return;
						}
						// item is a spellbook, learn new spell.
						DawnInterface::inscribeSpellInPlayerSpellbook(dynamic_cast<SpellActionBase*>(useItem->getItem()->getSpell()));
						GLfloat green[] = { 0.0f, 1.0f, 0.0f };
						DawnInterface::addTextToLogWindow(green, "You inscribed %s (rank %d) in your spellbook.", useItem->getItem()->getSpell()->getName().c_str(), useItem->getItem()->getSpell()->getRank());
						inventory->removeItem(useItem);
					}
					else {
						// item is potion or scroll, use it.
						if (m_player->castSpell(dynamic_cast<Spell*>(useItem->getItem()->getSpell()->cast(m_player, m_player, false))) == true) {
							useItem->decreaseCurrentStack();
							if (useItem->getCurrentStackSize() == 0) {
								inventory->removeItem(useItem);
							}
						}
					}
				}
				else if (floatingSelectionToHandle == NULL && useItem->canPlayerUseItem(*m_player) == true) {
					// try to equip the item
					std::vector<size_t> possibleSlots;
					for (size_t curSlotNr = 0; curSlotNr < static_cast<size_t>(Enums::ItemSlot::COUNTIS); ++curSlotNr) {
						Enums::ItemSlot curSlotEnum = static_cast<Enums::ItemSlot>(curSlotNr);
						if (Inventory::getEquipType(curSlotEnum) == useItem->getItem()->getEquipPosition()) {
							possibleSlots.push_back(curSlotNr);
						}
					}

					// find position to wield item
					size_t usedSlot = static_cast<size_t>(Enums::ItemSlot::COUNTIS);
					for (size_t curSlotIndex = 0; curSlotIndex < possibleSlots.size(); ++curSlotIndex) {
						if ((inventory->getItemAtSlot(static_cast<Enums::ItemSlot>(possibleSlots[curSlotIndex])) == NULL)
							|| (usedSlot == static_cast<size_t>(Enums::ItemSlot::COUNTIS))) {
							usedSlot = possibleSlots[curSlotIndex];
						}
					}

					if (usedSlot != static_cast<size_t>(Enums::ItemSlot::COUNTIS)) {
						// found a position. Insert item'
						Enums::ItemSlot curSlotEnum = static_cast<Enums::ItemSlot>(usedSlot);

						// special handler for when we are trying to wield a two-handed weapon and having items in BOTH mainhand and offhand-slot equipped.
						if (useItem->getItem()->isTwoHandedWeapon() == true && inventory->isWieldingTwoHandedWeapon() == false && inventory->getItemAtSlot(Enums::ItemSlot::MAIN_HAND) != NULL && inventory->getItemAtSlot(Enums::ItemSlot::OFF_HAND) != NULL) {
							if (inventory->insertItem(inventory->getItemAtSlot(Enums::ItemSlot::OFF_HAND)->getItem()) == true) {
								InventoryItem* tmp = inventory->getItemAtSlot(curSlotEnum);
								inventory->removeItem(useItem);
								inventory->wieldItemAtSlot(curSlotEnum, useItem);
								//CommonSounds::playClickSound();
								if (tmp != NULL && inventory->insertItem(tmp->getItem())) {
									delete tmp;
								}
								else {
									floatingSelection = tmp;
								}
							}
						}
						else if (useItem->getItem()->isTwoHandedWeapon() == true && inventory->isWieldingTwoHandedWeapon() == false && inventory->getItemAtSlot(Enums::ItemSlot::OFF_HAND) != NULL) {
							// special handler for when we are trying to wield a two-handed weapon and having ONLY an item in offhand-slot equipped.
							InventoryItem* tmp = inventory->getItemAtSlot(Enums::ItemSlot::OFF_HAND);
							inventory->removeItem(useItem);
							inventory->wieldItemAtSlot(curSlotEnum, useItem);
							//CommonSounds::playClickSound();
							if (tmp != NULL && inventory->insertItem(tmp->getItem())) {
								delete tmp;
							}
							else {
								floatingSelection = tmp;
							}
						}
						else {
							InventoryItem* tmp = inventory->getItemAtSlot(curSlotEnum);
							inventory->removeItem(useItem);
							inventory->wieldItemAtSlot(curSlotEnum, useItem);
							//CommonSounds::playClickSound();
							if (tmp != NULL && inventory->insertItem(tmp->getItem())) {
								delete tmp;
							}
							else {
								floatingSelection = tmp;
							}
						}
					}
				}
			}
			return;
		}

		if (floatingSelectionToHandle != NULL) {
			if (inventory->hasSufficientSpaceWithExchangeAt(fieldIndexX, fieldIndexY, floatingSelectionToHandle->getSizeX(), floatingSelectionToHandle->getSizeY())) {
				if (shopFloatingSelection) {
					floatingSelectionToHandle = new InventoryItem(floatingSelectionToHandle->getItem(), 0, 0, floatingSelectionToHandle);
				}
				floatingSelection = inventory->insertItemWithExchangeAt(floatingSelectionToHandle, fieldIndexX, fieldIndexY);
				if (shopFloatingSelection) {
					ShopCanvas::Get().getShop()->buyFromShop();
				}
				else {
					//CommonSounds::playClickSound();
				}
			}
		}
		else if (!inventory->isPositionFree(fieldIndexX, fieldIndexY)) {
			floatingSelection = inventory->getItemAt(fieldIndexX, fieldIndexY);
			inventory->removeItem(floatingSelection);
			//CommonSounds::playClickSound();
		}

	}
}

void InventoryCanvas::setTextureDependentPositions() {
	m_posX = ViewPort::get().getWidth() - m_textures[0].width - 50;
}

void InventoryCanvas::dropItemOnGround(InventoryItem* inventoryItem) {
	ZoneManager::Get().getCurrentZone()->getGroundLoot()->addItem(m_player->getXPos(), m_player->getYPos(), inventoryItem->getItem());
}

void InventoryCanvas::addInventoryScreenSlot(InventoryScreenSlot** mySlots, Enums::ItemSlot slotToUse, size_t offsetX, size_t offsetY, size_t sizeX, size_t sizeY, TextureRect texture) {
	mySlots[static_cast<size_t>(slotToUse)] = new InventoryScreenSlot(slotToUse, offsetX, offsetY, sizeX, sizeY, texture);
}

bool InventoryCanvas::isOnBackpackScreen(int mouseX, int mouseY) const {
	if (mouseX < static_cast<int>(m_posX + backpackOffsetX) ||
		mouseY < static_cast<int>(m_posY + backpackOffsetY) ||
		mouseX > static_cast<int>(m_posX + backpackOffsetX + backpackFieldWidth * numSlotsX + (numSlotsX - 1)*backpackSeparatorWidth) ||
		mouseY > static_cast<int>(m_posY + backpackOffsetY + backpackFieldHeight * numSlotsY + (numSlotsY - 1)*backpackSeparatorHeight)) {
		return false;
	}
	return true;
}

bool InventoryCanvas::isOverSlot(Enums::ItemSlot itemSlot, int mouseX, int mouseY) const {
	InventoryScreenSlot* curSlot = mySlots[static_cast<size_t>(itemSlot)];
	if (mouseX < static_cast<int>(m_posX + curSlot->getOffsetX()) ||
		mouseY < static_cast<int>(m_posY + curSlot->getOffsetY()) ||
		mouseX > static_cast<int>(m_posX + curSlot->getOffsetX() + curSlot->getSizeX()) ||
		mouseY > static_cast<int>(m_posY + curSlot->getOffsetY() + curSlot->getSizeY())) {
		return false;
	}
	return true;
}

bool InventoryCanvas::hasFloatingSelection() const {
	return floatingSelection != NULL;
}

void InventoryCanvas::setFloatingSelection(InventoryItem* item) {
	floatingSelection = item;
}

void InventoryCanvas::unsetFloatingSelection() {
	delete floatingSelection;
	floatingSelection = NULL;
}

InventoryItem* InventoryCanvas::getFloatingSelection() const {
	return floatingSelection;
}

Enums::ItemSlot InventoryCanvas::getMouseOverSlot(int mouseX, int mouseY) const {

	if (isOverSlot(Enums::ItemSlot::AMULET, mouseX, mouseY))    return Enums::ItemSlot::AMULET;
	if (isOverSlot(Enums::ItemSlot::BELT, mouseX, mouseY))      return Enums::ItemSlot::BELT;
	if (isOverSlot(Enums::ItemSlot::BOOTS, mouseX, mouseY))     return Enums::ItemSlot::BOOTS;
	if (isOverSlot(Enums::ItemSlot::CHEST, mouseX, mouseY))     return Enums::ItemSlot::CHEST;
	if (isOverSlot(Enums::ItemSlot::CLOAK, mouseX, mouseY))     return Enums::ItemSlot::CLOAK;
	if (isOverSlot(Enums::ItemSlot::GLOVES, mouseX, mouseY))    return Enums::ItemSlot::GLOVES;
	if (isOverSlot(Enums::ItemSlot::HEAD, mouseX, mouseY))      return Enums::ItemSlot::HEAD;
	if (isOverSlot(Enums::ItemSlot::LEGS, mouseX, mouseY))      return Enums::ItemSlot::LEGS;
	if (isOverSlot(Enums::ItemSlot::MAIN_HAND, mouseX, mouseY)) return Enums::ItemSlot::MAIN_HAND;
	if (isOverSlot(Enums::ItemSlot::OFF_HAND, mouseX, mouseY))  return Enums::ItemSlot::OFF_HAND;
	if (isOverSlot(Enums::ItemSlot::RING_ONE, mouseX, mouseY))  return Enums::ItemSlot::RING_ONE;
	if (isOverSlot(Enums::ItemSlot::RING_TWO, mouseX, mouseY))  return Enums::ItemSlot::RING_TWO;
	if (isOverSlot(Enums::ItemSlot::SHOULDER, mouseX, mouseY))  return Enums::ItemSlot::SHOULDER;
	return Enums::ItemSlot::COUNTIS;
}

void InventoryCanvas::equipOnSlotOriginDependingAndPlaySound(Enums::ItemSlot slotToUse, InventoryItem* wieldItem, bool fromShop, InventoryItem *newFloatingSelection) {
	Inventory* inventory = m_player->getInventory();
	if (fromShop) {
		// shop self-destroys its inventory items. Copy to new inventory item.
		wieldItem = new InventoryItem(wieldItem->getItem(), 0, 0, wieldItem);
	}

	inventory->wieldItemAtSlot(slotToUse, wieldItem);
	floatingSelection = newFloatingSelection;
	if (fromShop) {
		//shopWindow->buyFromShop();
	}
	else {
		//CommonSounds::playClickSound();
	}
}


