#include "Shop.h"
#include "Inventory.h"
#include "Luainterface.h"

ShopCanvas ShopCanvas::s_instance;

ShopCanvas& ShopCanvas::Get() {
	return s_instance;
}


ShopCanvas::ShopCanvas() : Widget(30, 80, 454, 404, 13, 15) {
	
}

ShopCanvas::~ShopCanvas() { 

}

void ShopCanvas::init() {
	m_itemStackFont = &Globals::fontManager.get("verdana_12");

	TextureAtlasCreator::get().init(1024, 1024);
	TextureManager::Loadimage("res/interface/Shop/base.tga", 0, m_textures);
	TextureManager::Loadimage("res/white2x2pixel.png", 1, m_textures, true);
	TextureManager::Loadimage("res/interface/Shop/weapontab.tga", 2, m_textures);
	TextureManager::Loadimage("res/interface/Shop/armortab.tga", 3, m_textures);
	TextureManager::Loadimage("res/interface/Shop/misctab.tga", 4, m_textures);

	m_textureAtlas = TextureAtlasCreator::get().getAtlas();

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
	TextureManager::DrawTextureBatched(tabs[currentTab].tabimage, m_posX + tabs[currentTab].posX, m_posY + tabs[currentTab].posY, tabs[currentTab].width, tabs[currentTab].height, false, false);
}

void ShopCanvas::drawItems() {
	size_t numItems = shopkeeperInventory[currentTab].size();
	for (size_t curItemNr = 0; curItemNr<numItems; ++curItemNr) {
		InventoryItem *curInvItem = shopkeeperInventory[currentTab][curItemNr];
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
	m_shopCanvas.shopkeeperInventory = shopkeeperInventory;
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