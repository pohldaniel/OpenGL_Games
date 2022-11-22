#include "InventoryScreen.h"
#include "Inventory.h"

InventoryScreen InventoryScreen::s_instance;

InventoryScreen& InventoryScreen::Get() {
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

InventoryScreen::InventoryScreen() : Widget(0, 80, 469, 654) {

}

InventoryScreen::~InventoryScreen() {
	size_t count = static_cast<size_t>(Enums::ItemSlot::COUNTIS);
	for (size_t curSlot = 0; curSlot<count; ++curSlot) {
		delete mySlots[curSlot];
	}
	delete[] mySlots;
}

void InventoryScreen::setPlayer(Player* player) {
	m_player = player;
}

void InventoryScreen::init() {

	TextureAtlasCreator::get().init(1024, 1024);
	TextureManager::Loadimage("res/interface/inventory/base.tga", 0, m_textures);
	TextureManager::Loadimage("res/white2x2pixel.tga", 1, m_textures, true);
	TextureManager::Loadimage("res/interface/inventory/goldcoin.tga", 2, m_textures);
	TextureManager::Loadimage("res/interface/inventory/silvercoin.tga", 3, m_textures);
	TextureManager::Loadimage("res/interface/inventory/coppercoin.tga", 4, m_textures);


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

	//Spritesheet::Safe("interface", m_textureAtlas);	

	backpackFieldWidth = 32;
	backpackFieldHeight = 32;
	backpackSeparatorWidth = 3;
	backpackSeparatorHeight = 3;
	backpackOffsetX = 69;
	backpackOffsetY = 59;
	numSlotsX = 10;
	numSlotsY = 4;

	mySlots = new InventoryScreenSlot*[static_cast<size_t>(Enums::ItemSlot::COUNTIS)];
	for (size_t curSlotNr = 0; curSlotNr < static_cast<size_t>(Enums::ItemSlot::COUNTIS); ++curSlotNr) {
		mySlots[curSlotNr] = NULL;
	}

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

	m_visible = true;
	addMoveableFrame(454, 21, 15, 650);
	addCloseButton(22, 22, 447, 650);
}

void InventoryScreen::draw() {
	if (!m_visible) return;
	TextureManager::BindTexture(m_textureAtlas, true);
	TextureManager::DrawTexture(m_textures[0], m_posX, m_posY, false, false);

	drawCoins();
	drawBackpack();

	TextureManager::UnbindTexture(true);
}

void InventoryScreen::drawCoins() {
	// draws our coins in gold, silver and copper.
	std::string gold = currency::convertCoinsToString(currency::GOLD, m_player->getCoins());
	std::string silver = currency::convertCoinsToString(currency::SILVER, m_player->getCoins());
	std::string copper = currency::convertCoinsToString(currency::COPPER, m_player->getCoins());
	
	// gold coin
	TextureManager::DrawTexture(m_textures[2], m_posX + 167, m_posY + 308, false, false);
	// silver coin
	TextureManager::DrawTexture(m_textures[3], m_posX + 167, m_posY + 286, false, false);
	// copper coin
	TextureManager::DrawTexture(m_textures[4], m_posX + 167, m_posY + 264, false, false);

	Fontrenderer::Get().drawText(*m_coinsFont, m_posX + 160 - m_coinsFont->getWidth(gold), m_posY + 307, gold, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
	Fontrenderer::Get().drawText(*m_coinsFont, m_posX + 160 - m_coinsFont->getWidth(silver), m_posY + 285, silver, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
	Fontrenderer::Get().drawText(*m_coinsFont, m_posX + 160 - m_coinsFont->getWidth(copper), m_posY + 263, copper, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
}

void InventoryScreen::drawBackpack() {
	Inventory* inventory = m_player->getInventory();
	std::vector<InventoryItem*> items = inventory->getBackpackItems();
	size_t numItems = items.size();

	TextureManager::BindTexture(TextureManager::GetTextureAtlas("items"), true);

	for (size_t curItemNr = 0; curItemNr<numItems; ++curItemNr) {
				
		InventoryItem* curInvItem = items[curItemNr];
		Item* curItem = curInvItem->getItem();
		TextureRect* symbolTexture = curItem->getSymbolTexture();

		size_t invPosX = curInvItem->getInventoryPosX();
		size_t invPosY = curInvItem->getInventoryPosY();
		size_t sizeX = curItem->getSizeX();
		size_t sizeY = curItem->getSizeY();

		TextureManager::DrawTexture(*symbolTexture, m_posX + backpackOffsetX + invPosX * backpackFieldWidth + invPosX * backpackSeparatorWidth, m_posY + backpackOffsetY + invPosY * backpackFieldHeight + invPosY * backpackSeparatorHeight, backpackFieldWidth * sizeX + (sizeX - 1)*backpackSeparatorWidth, backpackFieldHeight * sizeY + (sizeY - 1)*backpackSeparatorHeight, false, false);		
		// if we have an item that is stackable, and the stacksize is more than 1, we draw that number.
		if (curInvItem->getCurrentStackSize() > 1)  {
			Fontrenderer::Get().drawText(*m_coinsFont, m_posX + backpackOffsetX + backpackFieldWidth - m_coinsFont->getWidth(std::to_string(curInvItem->getCurrentStackSize())) + invPosX * backpackFieldWidth + invPosX * backpackSeparatorWidth, m_posY + backpackOffsetY + invPosY * backpackFieldHeight + invPosY * backpackSeparatorHeight, std::to_string(curInvItem->getCurrentStackSize()), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
		}
	}
}

void InventoryScreen::processInput() {
	if (!m_visible) return;
	Widget::processInput();
}

void InventoryScreen::addInventoryScreenSlot(InventoryScreenSlot** mySlots, Enums::ItemSlot slotToUse, size_t offsetX, size_t offsetY, size_t sizeX, size_t sizeY, TextureRect texture) {
	mySlots[static_cast<size_t>(slotToUse)] = new InventoryScreenSlot(slotToUse, offsetX, offsetY, sizeX, sizeY, texture);
}

std::string currency::getLongTextString(std::uint32_t coins) {
	std::stringstream ss;
	ss.clear();

	std::uint32_t copper = 0, silver = 0, gold = 0;
	bool addComma = false;

	exchangeCoins(copper, silver, gold, coins);
	if (gold > 0) {
		ss << gold << " gold";
		addComma = true;
	}

	if (silver > 0) {
		if (addComma == true) {
			ss << ", ";
		}
		ss << silver << " silver";
		addComma = true;
	}

	if (copper > 0) {
		if (addComma == true) {
			ss << ", ";
		}
		ss << copper << " copper";
	}

	return ss.str();;
}

void currency::exchangeCoins(std::uint32_t &copper, std::uint32_t &silver, std::uint32_t &gold, std::uint32_t &coins) {
	// exchanging coins to copper coins.
	copper = coins % 100;
	coins -= copper;
	if (coins == 0)
		return;

	coins /= 100;
	silver = coins % 100;
	coins -= silver;
	if (coins == 0)
		return;

	gold = coins / 100;
}

std::string currency::convertCoinsToString(currency currency, std::uint32_t coins) {
	std::stringstream ss;
	std::string output;

	std::uint32_t copper = 0, silver = 0, gold = 0;

	exchangeCoins(copper, silver, gold, coins);

	switch (currency)
	{
	case currency::COPPER:
		ss << copper;
		break;
	case currency::SILVER:
		ss << silver;
		break;
	case currency::GOLD:
		ss << gold;
		break;
	};

	return ss.str();
}