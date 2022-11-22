#include <algorithm>
#include "Groundloot.h"
#include "Player.h"
#include "Item.h"
#include "Random.h"


void sGroundItems::loadTooltip() {
	tooltipXpos = xpos + (item->getSizeX() * 32) / 2 - tooltipWidth / 2;
	tooltipYpos = ypos + (item->getSizeY() * 32) / 2;

	GLfloat grey[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat yellow[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	GLfloat orange[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat red[] = { 1.0f, 0.0f, 0.0f, 1.0f };

	// setting the title of the tooltip based on item quality
	switch (item->getItemQuality()) {
		case Enums::ItemQuality::POOR:
			for (size_t x = 0; x < 4; x++)
				color[x] = grey[x];
			break;
		case Enums::ItemQuality::NORMAL:
			for (size_t x = 0; x < 4; x++)
				color[x] = white[x];
			break;
		case Enums::ItemQuality::ENHANCED:
			for (size_t x = 0; x < 4; x++)
				color[x] = yellow[x];
			break;
		case Enums::ItemQuality::RARE:
			for (size_t x = 0; x < 4; x++)
				color[x] = orange[x];
			break;
		case Enums::ItemQuality::LORE:
			for (size_t x = 0; x < 4; x++)
				color[x] = red[x];
			break;
	}
}

GroundLoot::GroundLoot(Player *player_) : player(player_), drawTooltips(false) {
	/*font = FontCache::getFontFromCache("data/verdana.ttf", 11);
	textures.LoadIMG("data/interface/tooltip/groundloot_background.tga", 0);
	textures.LoadIMG("data/interface/tooltip/groundloot_left.tga", 1);
	textures.LoadIMG("data/interface/tooltip/groundloot_right.tga", 2);*/
}

GroundLoot::~GroundLoot() {
}

void GroundLoot::enableTooltips() {
	drawTooltips = true;
}

void GroundLoot::disableTooltips() {
	drawTooltips = false;
}

bool GroundLoot::PickUpLoot(Player *player_, sGroundItems groundItem, size_t curItem) {
	// additional variables for making the code more readable
	int itemWidth = groundItem.item->getSizeX() * 32;
	int itemHeight = groundItem.item->getSizeY() * 32;
	int itemX = groundItem.xpos;
	int itemY = groundItem.ypos;
	int playerXSize = player_->getWidth() / 2;
	int playerYSize = player_->getHeight() - 50;
	int wGrade = itemWidth / 32;
	int hGrade = itemHeight / 32;
	int wDiff = 0;
	int hDiff = 0;
	int additionalDist = 70;

	// this is the range value the players horizontal centerpoint must be in
	switch (wGrade) {
		case 1: wDiff = itemWidth * 2 + additionalDist;  break; //32px
		case 2: wDiff = itemWidth + additionalDist;    break; //64px
		case 3: wDiff = itemWidth + additionalDist;    break; //96px
		default:                                     break; //??px
	}

	switch (hGrade) {
		case 1: hDiff = itemHeight * 2 + additionalDist;  break; //32px
		case 2: hDiff = itemHeight + additionalDist;    break; //64px
		case 3: hDiff = itemHeight + additionalDist;    break; //96px
		default:                                      break; //??px
	}

	// if the difference between these two points are in range you can loot
	//       horizontal centerpoint     horizontal centerpoint    difference range
	if (((player_->getXPos() + playerXSize) - itemX + itemWidth / 2)  < itemWidth + wDiff &&
		((player_->getXPos() + playerXSize) - itemX + itemWidth / 2)  > itemWidth - wDiff &&
		((player_->getYPos() + playerYSize) - itemY + itemHeight / 2) < itemHeight + hDiff &&
		((player_->getYPos() + playerYSize) - itemY + itemHeight / 2) > itemHeight - hDiff)
	{
		return true;
	}

	return false;
}

void GroundLoot::searchForItems(int x, int y) {
	for (size_t curItem = 0; curItem < groundItems.size(); curItem++) {
		// if we're holding left ALT down, we first check to see if player is looting by tooltips.
		if (drawTooltips) {
			if (x >= groundItems[curItem].tooltipXpos
				&& x <= static_cast<int>(groundItems[curItem].tooltipXpos + groundItems[curItem].tooltipWidth + 16)
				&& y >= groundItems[curItem].tooltipYpos
				&& y <= static_cast<int>(groundItems[curItem].tooltipYpos + 16)) {

				if (PickUpLoot(player, groundItems[curItem], curItem))
					lootItem(groundItems[curItem].item, curItem);
			}
		}else {
			if (x >= groundItems[curItem].xpos
				&& x <= static_cast<int>(groundItems[curItem].xpos + groundItems[curItem].item->getSizeX() * 32)
				&& y >= groundItems[curItem].ypos
				&& y <= static_cast<int>(groundItems[curItem].ypos + groundItems[curItem].item->getSizeY() * 32)) {

				if (PickUpLoot(player, groundItems[curItem], curItem))
					lootItem(groundItems[curItem].item, curItem);
			}
		}
	}
}

void GroundLoot::lootItem(Item *item, size_t itemIndex) {
	GLfloat blue[] = { 0.4f, 0.4f, 0.8f };
	GLfloat yellow[] = { 1.0f, 1.0f, 0.0f };
	if (dynamic_cast<GoldHeap*>(item) != NULL) {
		// some gold heap
		GoldHeap *goldHeap = dynamic_cast<GoldHeap*>(item);
		player->giveCoins(goldHeap->numCoins());
		//DawnInterface::addTextToLogWindow(blue, "You looted %s.", currency::getLongTextString(goldHeap->numCoins()).c_str());
		delete goldHeap;
		removeItem(itemIndex);

	} else {
		bool inserted = player->getInventory()->insertItem(item);
		if (inserted) {
			removeItem(itemIndex);
			//DawnInterface::addTextToLogWindow(blue, "You looted %s.", item->getName().c_str());
		} else {
			//DawnInterface::addTextToLogWindow(yellow, "Inventory is full.");
		}
	}
}

InventoryItem *GroundLoot::getFloatingSelection(int x, int y) {
	// search for items on the ground and return them to use as floating selection in the inventoryscreen.
	for (size_t curItem = 0; curItem < groundItems.size(); curItem++) {
		if (x >= groundItems[curItem].xpos &&
			x <= static_cast<int>(groundItems[curItem].xpos + groundItems[curItem].item->getSizeX() * 32) &&
			y >= groundItems[curItem].ypos &&
			y <= static_cast<int>(groundItems[curItem].ypos + groundItems[curItem].item->getSizeY() * 32)) {

			if (PickUpLoot(player, groundItems[curItem], curItem)) {
				InventoryItem *returnItem = new InventoryItem(groundItems[curItem].item, 0, 0, player);
				removeItem(curItem);
				return returnItem;
			}
		}
	}
	return NULL;
}

void GroundLoot::addItem(int x, int y, Item *newItem) {
	// we dont want items to be dropped at the exact the same position, therefor we do a little random here aswell.
	/*groundItems.push_back(sGroundItems(x + RNG::randomSizeT(-20, 20), y + RNG::randomSizeT(-20, 20), newItem, font->calcStringWidth(newItem->getName())));

	if (dynamic_cast<GoldHeap*>(newItem) != NULL) {

		int xoffset = 4;
		for (size_t i = 0; i < 3; i++) {
			groundItems[groundItems.size() - 1].itemValue[i] = currency::convertCoinsToString(static_cast<currency::currency>(i), dynamic_cast<GoldHeap*> (newItem)->numCoins());
			if (groundItems[groundItems.size() - 1].itemValue[i] != "0") {
				groundItems[groundItems.size() - 1].coinsOffset[i] = xoffset;
				groundItems[groundItems.size() - 1].coinsTextOffset[i] = font->calcStringWidth(groundItems[groundItems.size() - 1].itemValue[i]);
				xoffset = xoffset + 25 + groundItems[groundItems.size() - 1].coinsTextOffset[i];
				groundItems[groundItems.size() - 1].tooltipWidth = xoffset - 25;
			}
		}
		groundItems[groundItems.size() - 1].tooltipWidth += 8;
	}
	sortItems();*/
}

void GroundLoot::removeItem(size_t pos) {
	groundItems.erase(groundItems.begin() + pos);
	sortItems();
}

void GroundLoot::removeItem(Item *item) {

	for (size_t itemIndex = 0; itemIndex < groundItems.size(); itemIndex++) {
		if (groundItems[itemIndex].item == item) {
			groundItems.erase(groundItems.begin() + itemIndex);
			sortItems();
		}
	}
}

bool groundItemYPosCompareFunction(const sGroundItems &item1, const sGroundItems &item2) {
	if (item1.tooltipYpos != item2.tooltipYpos) {
		return item1.tooltipYpos < item2.tooltipYpos;
	}else if (item1.tooltipXpos != item2.tooltipXpos) {
		return item1.tooltipXpos < item2.tooltipXpos;
	}else {
		// compare pointers to have consistent ordering of identical items
		return &item1 < &item2;
	}
}

void GroundLoot::sortItems() {
	// first restore original item position so tooltips always stay close to the items
	for (size_t curItem = 0; curItem<groundItems.size(); ++curItem) {
		groundItems[curItem].tooltipYpos = groundItems[curItem].ypos;
	}

	// sort by Y-Position. This gives an inital sorting
	std::sort(groundItems.begin(), groundItems.end(), groundItemYPosCompareFunction);

	// this needs to be added because drawing tooltipWidth isn't exact and height not set
	size_t tooltipAddSpaceX = m_textures[0].width + m_textures[2].width - 16;
	size_t tooltipAddSpaceY = m_textures[1].height;

	// Check each item for collision against already placed items and move up until it no longer collides
	for (size_t curItemNr = 1; curItemNr < groundItems.size(); ++curItemNr) {
		sGroundItems &curItem = groundItems[curItemNr];
		bool fitsSpace;
		do
		{
			fitsSpace = true;
			for (size_t previousItemNr = 0; previousItemNr < curItemNr; ++previousItemNr) {
				sGroundItems &prevItem = groundItems[previousItemNr];
				// check for overlap

				if (Character::HasIntersection(curItem.tooltipXpos,
					curItem.tooltipXpos + curItem.tooltipWidth + tooltipAddSpaceX,
					curItem.tooltipYpos,
					curItem.tooltipYpos + tooltipAddSpaceY,
					prevItem.tooltipXpos,
					prevItem.tooltipXpos + prevItem.tooltipWidth + tooltipAddSpaceX,
					prevItem.tooltipYpos,
					prevItem.tooltipYpos + tooltipAddSpaceY)) {
					fitsSpace = false;
				}
			}

			if (!fitsSpace) {
				curItem.tooltipYpos++;
			}
		} while (!fitsSpace);
	}
}

void GroundLoot::draw() {
	for (size_t curItem = 0; curItem < groundItems.size(); curItem++) {
		/*DrawingHelpers::mapTextureToRect(groundItems[curItem].item->getSymbolTexture()->getTexture(0),
			groundItems[curItem].xpos,
			groundItems[curItem].item->getSizeX() * 32,
			groundItems[curItem].ypos,
			groundItems[curItem].item->getSizeY() * 32);*/
	}
}

void GroundLoot::drawTooltip() {

	/*if (drawTooltips) {
		for (size_t curItem = 0; curItem < groundItems.size(); curItem++)
		{
			if (groundItems[curItem].tooltipXpos < mouseX + world_x &&
				groundItems[curItem].tooltipXpos + groundItems[curItem].tooltipWidth + 16 > mouseX + world_x &&
				groundItems[curItem].tooltipYpos < mouseY + world_y &&
				groundItems[curItem].tooltipYpos + 16 > mouseY + world_y)
			{
				glColor4f(0.1f, 0.1f, 0.1f, 1.0f);
			}
			else {
				glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
			}

			// left border
			DrawingHelpers::mapTextureToRect(textures.getTexture(1),
				groundItems[curItem].tooltipXpos,
				textures.getTexture(1).width,
				groundItems[curItem].tooltipYpos,
				textures.getTexture(1).height);
			// background
			DrawingHelpers::mapTextureToRect(textures.getTexture(0),
				groundItems[curItem].tooltipXpos + 16,
				groundItems[curItem].tooltipWidth - 16,
				groundItems[curItem].tooltipYpos,
				textures.getTexture(0).height);
			// right border
			DrawingHelpers::mapTextureToRect(textures.getTexture(2),
				groundItems[curItem].tooltipXpos + groundItems[curItem].tooltipWidth,
				textures.getTexture(2).width,
				groundItems[curItem].tooltipYpos,
				textures.getTexture(2).height);

			glColor4fv(groundItems[curItem].color);
			if (dynamic_cast<GoldHeap*>(groundItems[curItem].item) == NULL)
			{
				font->drawText(groundItems[curItem].tooltipXpos + 10,
					groundItems[curItem].tooltipYpos + 2,
					groundItems[curItem].item->getName());
			}
			else {
				for (size_t i = 0; i < 3; i++)
				{
					if (groundItems[curItem].itemValue[i] != "0")
					{
						DrawFunctions::drawCoin(groundItems[curItem].tooltipXpos + groundItems[curItem].tooltipWidth - groundItems[curItem].coinsOffset[i], groundItems[curItem].tooltipYpos + 2, i);
						font->drawText(groundItems[curItem].tooltipXpos + groundItems[curItem].tooltipWidth - groundItems[curItem].coinsOffset[i] - groundItems[curItem].coinsTextOffset[i], groundItems[curItem].tooltipYpos + 2, groundItems[curItem].itemValue[i]);
					}
				}
			}
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}*/
}