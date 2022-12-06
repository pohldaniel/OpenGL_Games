#include "Shop.h"
#include "Inventory.h"
#include "InventoryCanvas.h"
#include "Luainterface.h"

ShopCanvas ShopCanvas::s_instance;

ShopCanvas& ShopCanvas::Get() {
	return s_instance;
}

ShopCanvas::ShopCanvas() : Widget(30, 80, 454, 404, 13, 15) {
	
}

ShopCanvas::~ShopCanvas() { 

}

void ShopCanvas::setShop(Shop* shop) {
	m_shop = shop;
}

Shop* ShopCanvas::getShop() {
	return m_shop;
}

void ShopCanvas::init() {
	m_itemStackFont = &Globals::fontManager.get("verdana_12");

	TextureAtlasCreator::Get().init("shop", 1024, 1024);
	TextureManager::Loadimage("res/interface/Shop/base.tga", 0, m_textures);
	TextureManager::Loadimage("res/white2x2pixel.png", 1, m_textures, true);
	TextureManager::Loadimage("res/interface/Shop/weapontab.tga", 2, m_textures);
	TextureManager::Loadimage("res/interface/Shop/armortab.tga", 3, m_textures);
	TextureManager::Loadimage("res/interface/Shop/misctab.tga", 4, m_textures);

	m_textureAtlas = TextureAtlasCreator::Get().getAtlas();

	currentTab = 0;
	tabs[0].tabimage = m_textures[2];
	tabs[0].height = 128;
	tabs[0].width = 128;

	tabs[0].posX = 61;
	tabs[0].posY = 264;

	tabs[1].tabimage = m_textures[3];
	tabs[1].height = 128;
	tabs[1].width = 128;
	tabs[1].posX = 202;
	tabs[1].posY = 264;

	tabs[2].tabimage = m_textures[4];
	tabs[2].height = 128;
	tabs[2].width = 128;
	tabs[2].posX = 343;
	tabs[2].posY = 264;

	addMoveableFrame(454, 21, 13, 398);
	addCloseButton(22, 22, 444, 398);

	backpackFieldWidth = 32;
	backpackFieldHeight = 32;
	backpackSeparatorWidth = 3;
	backpackSeparatorHeight = 3;
	backpackOffsetX = 67;
	backpackOffsetY = 56;
}

void ShopCanvas::draw() {
	if (!m_visible) return;
	TextureManager::BindTexture(m_textureAtlas, true, 0);
	TextureManager::DrawTexture(m_textures[0], m_posX, m_posY, false, false);
	drawTabs();
	drawItems();
	//TextureManager::DrawBuffer(false);
}

void ShopCanvas::drawTabs() {
	TextureManager::DrawTexture(tabs[currentTab].tabimage, m_posX + tabs[currentTab].posX, m_posY + tabs[currentTab].posY, tabs[currentTab].width, tabs[currentTab].height, false, false);
}

void ShopCanvas::drawItems() {
	size_t numItems = m_shop->shopkeeperInventory[currentTab].size();
	for (size_t curItemNr = 0; curItemNr<numItems; ++curItemNr) {
		InventoryItem *curInvItem = m_shop->shopkeeperInventory[currentTab][curItemNr];
		Item *curItem = curInvItem->getItem();
		TextureRect* symbolTexture = curItem->getSymbolTexture();

		size_t invPosX = curInvItem->getInventoryPosX();
		size_t invPosY = curInvItem->getInventoryPosY();
		size_t sizeX = curItem->getSizeX();
		size_t sizeY = curItem->getSizeY();

		Vector4f shade = Vector4f(0.0f, 0.0f, 0.0f, 0.1f);

		// if the item is equippable for the player and if we can afford it,
		// we draw a green backdrop. If not, a red backdrop.
		if (curInvItem->canPlayerUseItem(Player::Get()) == true && (Player::Get().getCoins() >= curItem->getValue())) {
			shade[1] = 1.0f; // green color
		}else {
			shade[0] = 1.0f; // red color
		}
		TextureManager::BindTexture(m_textureAtlas, true, 0);
		TextureManager::DrawTexture(m_textures[1],
			m_posX + backpackOffsetX + invPosX * backpackFieldWidth + invPosX * backpackSeparatorWidth,
			m_posY + backpackOffsetY + invPosY * backpackFieldHeight + invPosY * backpackSeparatorHeight,
			backpackFieldWidth * sizeX + (sizeX - 1)*backpackSeparatorWidth,
			backpackFieldHeight * sizeY + (sizeY - 1)*backpackSeparatorHeight,
			shade,
			false,
			false);

		TextureManager::BindTexture(TextureManager::GetTextureAtlas("items"), true, 0);
		TextureManager::DrawTexture(*symbolTexture,
			m_posX + backpackOffsetX + invPosX * backpackFieldWidth + invPosX * backpackSeparatorWidth,
			m_posY + backpackOffsetY + invPosY * backpackFieldHeight + invPosY * backpackSeparatorHeight,
			backpackFieldWidth * sizeX + (sizeX - 1)*backpackSeparatorWidth,
			backpackFieldHeight * sizeY + (sizeY - 1)*backpackSeparatorHeight,
			false,
			false);
		TextureManager::UnbindTexture(true);
	
		// if we have an item that is stackable, and the stacksize is more than 1, we draw that number.
		if (curInvItem->getCurrentStackSize() > 1) {
			Fontrenderer::Get().addText(*m_itemStackFont,
				m_posX + backpackOffsetX + backpackFieldWidth - m_itemStackFont->getWidth(std::to_string(curInvItem->getCurrentStackSize())) + invPosX * backpackFieldWidth + invPosX * backpackSeparatorWidth,
				m_posY + backpackOffsetY + invPosY * backpackFieldHeight + invPosY * backpackSeparatorHeight,
				std::to_string(curInvItem->getCurrentStackSize()));
		}
	}
}

void ShopCanvas::drawItemTooltip(int mouseX, int mouseY) {
	if (!m_visible) return;
	// draws tooltip over item in the shop
	if (isOnSlotsScreen(mouseX, mouseY) && isVisible() && floatingSelection == NULL) {
		InventoryItem *tooltipItem;
		int fieldIndexX = (mouseX - (m_posX + backpackOffsetX)) / (backpackFieldWidth + backpackSeparatorWidth);
		int fieldIndexY = (mouseY - (m_posY + backpackOffsetY)) / (backpackFieldHeight + backpackSeparatorHeight);

		if (!isPositionFree(fieldIndexX, fieldIndexY, currentTab)) {
			tooltipItem = getItemAt(fieldIndexX, fieldIndexY, currentTab);
			tooltipItem->getTooltip()->setShopItem(true);
			tooltipItem->getTooltip()->draw(mouseX, mouseY);

			//if player is holding down right shift and has an
			// item with same slot equipped, we draw that too.
			int thisTooltipPosX;
			int previousFrameHeight;
			bool firstItemCompared = false;
			Keyboard &keyboard = Keyboard::instance();
			if (keyboard.keyDown(Keyboard::KEY_LSHIFT)) {
				std::vector<InventoryItem*> equippedItems = m_shop->m_player->getInventory()->getEquippedItems();
				for (size_t curItem = 0; curItem < equippedItems.size(); curItem++) {

					if (equippedItems[curItem]->getItem()->getEquipPosition() == tooltipItem->getItem()->getEquipPosition()) {
						int thisTooltipPosY = mouseY;
						// if this is our second item we're adding to the compare, then we need to position it next to the previous tooltip.
						if (firstItemCompared == true) {
							thisTooltipPosY += previousFrameHeight + 30;
						}

						// if this is the first (or only) item we're going to draw in the compare we check where it will fit.
						if (firstItemCompared == false) {
							if (ViewPort::Get().getWidth() - (mouseX + tooltipItem->getTooltip()->getTooltipWidth() + 60) > equippedItems[curItem]->getTooltip()->getTooltipWidth()) {
								thisTooltipPosX = mouseX + tooltipItem->getTooltip()->getTooltipWidth() + 30;
							}else {
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
}

void ShopCanvas::drawFloatingSelection() {
	// draw floating selection
	if (hasFloatingSelection()) {
		Item *floatingItem = floatingSelection->getItem();
		size_t sizeX = floatingItem->getSizeX();
		size_t sizeY = floatingItem->getSizeY();

		TextureManager::BindTexture(TextureManager::GetTextureAtlas("items"), true);
		TextureManager::DrawTexture(*floatingItem->getSymbolTexture(), ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY() - 20, backpackFieldWidth * sizeX + (sizeX - 1) * backpackSeparatorWidth, backpackFieldHeight * sizeY + (sizeY - 1) * backpackSeparatorHeight, false, false);
		TextureManager::UnbindTexture(true);
	}
}

void ShopCanvas::processInput() {
	if (!m_visible) return;
	Widget::processInput();

	Mouse &mouse = Mouse::instance();
	if (mouse.buttonPressed(Mouse::BUTTON_LEFT) || mouse.buttonPressed(Mouse::BUTTON_RIGHT)) {
		if (isOnSlotsScreen(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY())) {

			if (InventoryCanvas::Get().hasFloatingSelection()) {
				m_shop->sellToShop(InventoryCanvas::Get().getFloatingSelection(), true);
				InventoryCanvas::Get().unsetFloatingSelection();
			}

			if (hasFloatingSelection()) {
				m_shop->sellToShop(getFloatingSelection(), false);
				unsetFloatingSelection();
				return;
			}

			int fieldIndexX = (ViewPort::Get().getCursorPosRelX() - (m_posX + backpackOffsetX)) / (backpackFieldWidth + backpackSeparatorWidth);
			int fieldIndexY = (ViewPort::Get().getCursorPosRelY() - (m_posY + backpackOffsetY)) / (backpackFieldHeight + backpackSeparatorHeight);

			if (!isPositionFree(fieldIndexX, fieldIndexY, currentTab)) {
				
				InventoryItem *curItem = getItemAt(fieldIndexX, fieldIndexY, currentTab);
				if (m_shop->m_player->getCoins() >= curItem->getItem()->getValue()) {
					if (mouse.buttonPressed(Mouse::BUTTON_RIGHT)) {
						// direct buy on right button
						bool inserted = m_shop->m_player->getInventory()->insertItem(curItem->getItem(), curItem);
						if (inserted) {
							floatingSelection = curItem;
							m_shop->removeItem(floatingSelection);
							m_shop->buyFromShop();
						}
					}else {
						
						// add pick up item on left click
						floatingSelection = curItem;
						m_shop->removeItem(floatingSelection);
					}
				}
			}
			return;
		}
	
		// loop through our tabs, see if any got clicked.
		for (size_t tabIndex = 0; tabIndex <= 2; tabIndex++) {
			if (ViewPort::Get().getCursorPosRelX() > tabs[tabIndex].posX + m_posX
				&& ViewPort::Get().getCursorPosRelY() > tabs[tabIndex].posY + m_posY
				&& ViewPort::Get().getCursorPosRelX() < tabs[tabIndex].posX + m_posX + tabs[tabIndex].width
				&& ViewPort::Get().getCursorPosRelY() < tabs[tabIndex].posY + m_posY + tabs[tabIndex].height) {
				currentTab = tabIndex;
				return;
			}
		}

		if (!isOnSlotsScreen(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY())) {
			if (hasFloatingSelection()) {
				m_shop->sellToShop(getFloatingSelection(), false);
				unsetFloatingSelection();
				return;
			}
		}
	}
}

InventoryItem* ShopCanvas::getItemAt(size_t invPosX, size_t invPosY, size_t itemTab) {

	size_t numBackItems = m_shop->shopkeeperInventory[itemTab].size();
	for (size_t curBackItemNr = 0; curBackItemNr<numBackItems; ++curBackItemNr) {
		InventoryItem *curItem = m_shop->shopkeeperInventory[itemTab][curBackItemNr];
		size_t itemPosX = curItem->getInventoryPosX();
		size_t itemPosY = curItem->getInventoryPosY();
		size_t itemSizeX = curItem->getItem()->getSizeX();
		size_t itemSizeY = curItem->getItem()->getSizeY();

		if (itemPosX <= invPosX && itemPosX + itemSizeX > invPosX
			&& itemPosY <= invPosY && itemPosY + itemSizeY > invPosY) {
			return curItem;
		}
	}
}

bool ShopCanvas::isOnSlotsScreen(int x, int y) {

	if (x < static_cast<int>(m_posX + backpackOffsetX)
		|| y < static_cast<int>(m_posY + backpackOffsetY)
		|| x > static_cast<int>(m_posX + backpackOffsetX + backpackFieldWidth * m_shop->numSlotsX + (m_shop->numSlotsX - 1)*backpackSeparatorWidth)
		|| y > static_cast<int>(m_posY + backpackOffsetY + backpackFieldHeight * m_shop->numSlotsY + (m_shop->numSlotsY - 1)*backpackSeparatorHeight)) {
		return false;
	}
	return true;
}

bool ShopCanvas::isPositionFree(size_t invPosX, size_t invPosY, size_t curTab) const {

	if (invPosX >= m_shop->numSlotsX || invPosY >= m_shop->numSlotsY) {
		return false;
	}

	return (!m_shop->slotUsed[curTab][invPosX][invPosY]);
}

bool ShopCanvas::hasFloatingSelection() const {
	return floatingSelection != NULL;
}

void ShopCanvas::unsetFloatingSelection(){
	delete floatingSelection;
	floatingSelection = NULL;
}

InventoryItem *ShopCanvas::getFloatingSelection() const {
	return floatingSelection;
}
/////////////////////////////////////////////////////////////
Shop::Shop() : m_shopCanvas(ShopCanvas::Get()) {
	numSlotsX = 10;
	numSlotsY = 6;
	
	slotUsed = new bool**[3];
	for (size_t curItemTab = 0; curItemTab<3; ++curItemTab) {
		slotUsed[curItemTab] = new bool*[numSlotsX];
		for (size_t curX = 0; curX<numSlotsX; ++curX) {
			slotUsed[curItemTab][curX] = new bool[numSlotsY];
			for (size_t curY = 0; curY<numSlotsY; ++curY) {
				slotUsed[curItemTab][curX][curY] = false;
			}
		}
	}
}

Shop::~Shop() {}

void Shop::setPlayer(Player* player) {
	m_player = player;
}

void Shop::addItem(Item *item) {
	InventoryItem invItem(item, 0, 0);
	sellToShop(&invItem, false);
}

void Shop::loadShopkeeperInventory() {
	m_player = &Player::Get();
	m_shopCanvas.setShop(this);
}

void Shop::sellToShop(InventoryItem *sellItem, bool givePlayerMoney) {

	Item *item = sellItem->getItem();

	size_t itemSizeX = item->getSizeX();
	size_t itemSizeY = item->getSizeY();

	size_t itemTab = getItemTab(item);

	bool foundPosition = false;
	size_t foundX = 0;
	size_t foundY = 0;

	// look for next free position
	for (size_t freeX = 0; freeX < numSlotsX - itemSizeX + 1 && !foundPosition; ++freeX) {
		for (size_t freeY = 0; freeY< numSlotsY - itemSizeY + 1 && !foundPosition; ++freeY) {
			if (hasSufficientSpaceAt(freeX, freeY, itemSizeX, itemSizeY, itemTab)) {
				foundPosition = true;
				foundX = freeX;
				foundY = freeY;
			}
		}
	}

	if (foundPosition) {
		InventoryItem *newItem = new InventoryItem(item, foundX, foundY, sellItem);
		insertItemAt(newItem, foundX, foundY, itemTab);
	}

	if (givePlayerMoney) {
		//SoundEngine::playSound("data/sound/sell_buy_item.ogg");
		GLfloat yellow[] = { 1.0f, 1.0f, 0.0f };
		m_player->giveCoins(floor(sellItem->getItem()->getValue() * 0.75) * sellItem->getCurrentStackSize());
		if (sellItem->getCurrentStackSize() > 1) {
			DawnInterface::addTextToLogWindow(yellow, "Sold %d %s.", sellItem->getCurrentStackSize(), item->getName().c_str());
		}else {
			DawnInterface::addTextToLogWindow(yellow, "Sold %s.", item->getName().c_str());
		}
		// player only gets 75% of the itemvalue when he sells an item
	}
}

bool Shop::hasSufficientSpaceAt(size_t inventoryPosX, size_t inventoryPosY, size_t itemSizeX, size_t itemSizeY, size_t itemTab) const {

	size_t inventoryMaxX = inventoryPosX + itemSizeX - 1;
	size_t inventoryMaxY = inventoryPosY + itemSizeY - 1;
	if ((inventoryMaxX >= numSlotsX) || (inventoryMaxY >= numSlotsY)) {
		return false;
	}


	for (size_t curX = inventoryPosX; curX <= inventoryMaxX; ++curX) {
		for (size_t curY = inventoryPosY; curY <= inventoryMaxY; ++curY) {
			if (slotUsed[itemTab][curX][curY]) {
				return false;
			}
		}
	}

	return true;
}

void Shop::insertItemAt(InventoryItem *inventoryItem, size_t invPosX, size_t invPosY, size_t itemTab) {

	inventoryItem->setInventoryPos(invPosX, invPosY);
	shopkeeperInventory[itemTab].push_back(inventoryItem);

	size_t inventoryMaxX = invPosX + inventoryItem->getSizeX() - 1;
	size_t inventoryMaxY = invPosY + inventoryItem->getSizeY() - 1;

	for (size_t curX = invPosX; curX <= inventoryMaxX; ++curX) {
		for (size_t curY = invPosY; curY <= inventoryMaxY; ++curY) {
			slotUsed[itemTab][curX][curY] = true;
		}
	}
}

void Shop::buyFromShop() {
	m_player->reduceCoins(m_shopCanvas.getFloatingSelection()->getItem()->getValue() * m_shopCanvas.getFloatingSelection()->getCurrentStackSize());

	//SoundEngine::playSound("data/sound/sell_buy_item.ogg");

	GLfloat yellow[] = { 1.0f, 1.0f, 0.0f };
	if (m_shopCanvas.getFloatingSelection()->getCurrentStackSize() > 1) {
		DawnInterface::addTextToLogWindow(yellow, "Purchased %d %s.", m_shopCanvas.getFloatingSelection()->getCurrentStackSize(), m_shopCanvas.getFloatingSelection()->getItem()->getName().c_str());
	}else {
		DawnInterface::addTextToLogWindow(yellow, "Purchased %s.", m_shopCanvas.getFloatingSelection()->getItem()->getName().c_str());
	}

	m_shopCanvas.unsetFloatingSelection();
}

void Shop::removeItem(InventoryItem *inventoryItem)
{
	for (size_t curTab = 0; curTab < 3; ++curTab) {
		for (size_t curBackpackItemNr = 0; curBackpackItemNr<shopkeeperInventory[curTab].size(); ++curBackpackItemNr) {
			if (shopkeeperInventory[curTab][curBackpackItemNr] == inventoryItem) {
				for (size_t curX = inventoryItem->getInventoryPosX(); curX<inventoryItem->getInventoryPosX() + inventoryItem->getSizeX(); ++curX) {
					for (size_t curY = inventoryItem->getInventoryPosY(); curY<inventoryItem->getInventoryPosY() + inventoryItem->getSizeY(); ++curY) {
						slotUsed[curTab][curX][curY] = false;
					}
				}
				shopkeeperInventory[curTab][curBackpackItemNr] = shopkeeperInventory[curTab][shopkeeperInventory[curTab].size() - 1];
				shopkeeperInventory[curTab].resize(shopkeeperInventory[curTab].size() - 1);
			}
		}
	}
}

unsigned short Shop::getItemTab(Item *item) {
	switch (item->getItemType()) {
		case Enums::ItemType::MISCELLANEOUS:
			return 2;
			break;
		case Enums::ItemType::ARMOR:
			return 1;
			break;
		case Enums::ItemType::WEAPON:
			if (item->getWeaponType() == Enums::WeaponType::SHIELD) // this is to get all shields into the secondary tab.
			{
				return 1;
			}
			return 0;
			break;
		case Enums::ItemType::JEWELRY:
			return 2;
			break;
		case Enums::ItemType::SCROLL:
			return 2;
			break;
		case Enums::ItemType::POTION:
			return 2;
			break;
		case Enums::ItemType::FOOD:
			return 2;
			break;
		case Enums::ItemType::DRINK:
			return 2;
			break;
		case Enums::ItemType::NEWSPELL:
			return 2;
			break;
		default:
			return 0;
			break;
	}
}
////////////////////////////////////////////////////////////////////
ShopManager ShopManager::s_instance;

ShopManager& ShopManager::Get() {
	return s_instance;
}

Shop& ShopManager::getShop(std::string shopName) {
	if (m_shops.find(shopName) == m_shops.end()) {
		m_shops[shopName] = new Shop();
	}

	return  *m_shops[shopName];
}