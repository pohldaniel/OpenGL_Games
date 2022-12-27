#include <memory>

#include "Tooltip.h"
#include "Player.h"
#include "Item.h"
#include "Dialog.h"
#include "Statssystem.h"
#include "TextWindow.h"
#include "Utils.h"
#include "Constants.h"


Tooltip::~Tooltip() { }

void Tooltip::updateBlockNumbers() {
	if (smallTooltip) {
		curBlockNumberWidth = blockNumberWidthSmall;
		curBlockNumberHeight = blockNumberHeightSmall;

	}else {
		curBlockNumberWidth = blockNumberWidth;
		curBlockNumberHeight = blockNumberHeight;
	}
}

void Tooltip::enableSmallTooltip() {
	smallTooltip = true;
	reloadTooltip();
	updateBlockNumbers();
}

void Tooltip::disableSmallTooltip() {
	smallTooltip = false;
	reloadTooltip();
	updateBlockNumbers();
}

bool Tooltip::isTooltipSmall() {
	return smallTooltip;
}

void Tooltip::reloadTooltip() {
	shoppingState = player->isShopping();
	tooltipText.clear();
	getParentText();
}

int Tooltip::getTooltipWidth() const {
	return blockWidth * curBlockNumberWidth + blockWidth;
}

int Tooltip::getTooltipHeight() const {
	return blockHeight * curBlockNumberHeight + blockHeight;
}

void Tooltip::addTooltipText(Vector4f color, CharacterSet* charSet, std::string str, ...) {
	std::va_list args;
	char buf[1024];

	va_start(args, str);
	vsnprintf(buf, 1024, str.c_str(), args);
	va_end(args);

	// if it's just a newline we're adding then just add it without checking for multiline...
	if (str.empty() == true) {
		tooltipText.push_back(sTooltipText("", color, charSet));

	}else if (str.find("price:") != str.npos) {
		// else check to see if the text contains price information. if so we dont wordwrap.
		tooltipText.push_back(sTooltipText(buf, color, charSet));

	}else {
		// format the text into several lines so that the tooltip doesnt get too wide,
		//then push all the text lines to our vector.
		std::vector<std::string> formattedLines;
		TextWindow::FormatMultilineText(buf, formattedLines, 300, charSet);
		for (size_t curLine = 0; curLine < formattedLines.size(); curLine++) {
			tooltipText.push_back(sTooltipText(formattedLines[curLine], color, charSet));
		}
	}

	// adjust width and height depending on the content of the tooltip.
	int width = 0;
	int height = 0;
	int widthSmall = 0;
	int heightSmall = 0;
	toplineHeight = 0;
	if (tooltipText.size() > 0) {
		toplineHeight = tooltipText[0].charset->lineHeight;

		// adjusting position for the font when using a small tooltip here. so it get's more centered.. maybe a bit too static, not sure.
		if (smallTooltip == true) {
			toplineHeight += 10;
		}
	}

	for (unsigned int i = 0; i < tooltipText.size(); i++) {
		int neededWidth = tooltipText[i].charset->getWidth(tooltipText[i].text);

		if (width < neededWidth) {
			width = neededWidth;
		}
		// add line and line distance
		height += tooltipText[i].charset->lineHeight;
		if (i + 1 < tooltipText.size()) {
			height += 11;
		}

		if (i == 0) {
			widthSmall = width;
			heightSmall = height;
		}
	}

	blockNumberHeight = ceil(static_cast<double>(height) / blockHeight);
	blockNumberWidth = ceil(static_cast<double>(width) / blockWidth);

	blockNumberHeightSmall = ceil(static_cast<double>(heightSmall) / blockHeight);
	blockNumberWidthSmall = ceil(static_cast<double>(widthSmall) / blockWidth);

	updateBlockNumbers();
}

std::string Tooltip::getDynamicValues(SpellActionBase *spell, size_t val) const {
	std::stringstream ss;
	ss.str() = "";

	switch (val) {
	case 0: // minWeaponDamage
		if (dynamic_cast<MeleeDamageAction*>(spell) != NULL) {
			MeleeDamageAction *curSpell = dynamic_cast<MeleeDamageAction*>(spell);
			ss << static_cast<int16_t>(player->getModifiedMinDamage() * StatsSystem::getStatsSystem()->complexGetDamageModifier(player->getLevel(), player->getModifiedDamageModifierPoints(), player->getLevel()) * curSpell->getDamageBonus());
			return ss.str();
		}
		else if (dynamic_cast<RangedDamageAction*>(spell) != NULL) {
			RangedDamageAction *curSpell = dynamic_cast<RangedDamageAction*>(spell);
			ss << static_cast<int16_t>(player->getModifiedMinDamage() * StatsSystem::getStatsSystem()->complexGetDamageModifier(player->getLevel(), player->getModifiedDamageModifierPoints(), player->getLevel()) * curSpell->getDamageBonus());
			return ss.str();
		}
		break;
	case 1: // maxWeaponDamage
		if (dynamic_cast<MeleeDamageAction*>(spell) != NULL) {
			MeleeDamageAction *curSpell = dynamic_cast<MeleeDamageAction*>(spell);
			ss << static_cast<int16_t>(player->getModifiedMaxDamage() * StatsSystem::getStatsSystem()->complexGetDamageModifier(player->getLevel(), player->getModifiedDamageModifierPoints(), player->getLevel()) * curSpell->getDamageBonus());
			return ss.str();
		}
		else if (dynamic_cast<RangedDamageAction*>(spell) != NULL) {
			RangedDamageAction *curSpell = dynamic_cast<RangedDamageAction*>(spell);
			ss << static_cast<int16_t>(player->getModifiedMaxDamage() * StatsSystem::getStatsSystem()->complexGetDamageModifier(player->getLevel(), player->getModifiedDamageModifierPoints(), player->getLevel()) * curSpell->getDamageBonus());
			return ss.str();
		}
		break;
	case 2: // minSpellDirectDamage
		if (dynamic_cast<GeneralDamageSpell*>(spell) != NULL) {
			GeneralDamageSpell *curSpell = dynamic_cast<GeneralDamageSpell*>(spell);
			ss << static_cast<int16_t>(curSpell->getDirectDamageMin() * StatsSystem::getStatsSystem()->complexGetSpellEffectElementModifier(player->getLevel(), player->getModifiedSpellEffectElementModifierPoints(curSpell->getDirectDamageElement()), player->getLevel()));
			return ss.str();
		}
		break;
	case 3: // maxSpellDirectDamage
		if (dynamic_cast<GeneralDamageSpell*>(spell) != NULL) {
			GeneralDamageSpell *curSpell = dynamic_cast<GeneralDamageSpell*>(spell);
			ss << static_cast<int16_t>(curSpell->getDirectDamageMax() * StatsSystem::getStatsSystem()->complexGetSpellEffectElementModifier(player->getLevel(), player->getModifiedSpellEffectElementModifierPoints(curSpell->getDirectDamageElement()), player->getLevel()));
			return ss.str();
		}
		break;
	case 4: // minSpellContinuousDamage
		if (dynamic_cast<GeneralDamageSpell*>(spell) != NULL) {
			GeneralDamageSpell *curSpell = dynamic_cast<GeneralDamageSpell*>(spell);
			ss << static_cast<int16_t>(curSpell->getDuration() * curSpell->getContinuousDamageMin() * StatsSystem::getStatsSystem()->complexGetSpellEffectElementModifier(player->getLevel(), player->getModifiedSpellEffectElementModifierPoints(curSpell->getContinuousDamageElement()), player->getLevel()));
			return ss.str();
		}
		break;
	case 5: // maxSpellContinuousDamage
		if (dynamic_cast<GeneralDamageSpell*>(spell) != NULL) {
			GeneralDamageSpell *curSpell = dynamic_cast<GeneralDamageSpell*>(spell);
			ss << static_cast<int16_t>(curSpell->getDuration() * curSpell->getContinuousDamageMax() * StatsSystem::getStatsSystem()->complexGetSpellEffectElementModifier(player->getLevel(), player->getModifiedSpellEffectElementModifierPoints(curSpell->getContinuousDamageElement()), player->getLevel()));
			return ss.str();
		}
		break;
	case 6: // minDirectHealing
		if (dynamic_cast<GeneralHealingSpell*>(spell) != NULL) {
			GeneralHealingSpell *curSpell = dynamic_cast<GeneralHealingSpell*>(spell);
			ss << static_cast<int16_t>(curSpell->getDirectHealingMin() * StatsSystem::getStatsSystem()->complexGetSpellEffectElementModifier(player->getLevel(), player->getModifiedSpellEffectElementModifierPoints(curSpell->getDirectElementType()), player->getLevel()));
			return ss.str();
		}
		break;
	case 7: // maxDirectHealing
		if (dynamic_cast<GeneralHealingSpell*>(spell) != NULL) {
			GeneralHealingSpell *curSpell = dynamic_cast<GeneralHealingSpell*>(spell);
			ss << static_cast<int16_t>(curSpell->getDirectHealingMax() * StatsSystem::getStatsSystem()->complexGetSpellEffectElementModifier(player->getLevel(), player->getModifiedSpellEffectElementModifierPoints(curSpell->getDirectElementType()), player->getLevel()));
			return ss.str();
		}
		break;
	case 8: // minContinuousHealing
		if (dynamic_cast<GeneralHealingSpell*>(spell) != NULL) {
			GeneralHealingSpell *curSpell = dynamic_cast<GeneralHealingSpell*>(spell);
			ss << static_cast<int16_t>(curSpell->getDuration() * curSpell->getContinuousHealingMin() * StatsSystem::getStatsSystem()->complexGetSpellEffectElementModifier(player->getLevel(), player->getModifiedSpellEffectElementModifierPoints(curSpell->getContinuousElementType()), player->getLevel()));
			return ss.str();
		}
		break;
	case 9: // maxContinuousHealing
		if (dynamic_cast<GeneralHealingSpell*>(spell) != NULL) {
			GeneralHealingSpell *curSpell = dynamic_cast<GeneralHealingSpell*>(spell);
			ss << static_cast<int16_t>(curSpell->getDuration() * curSpell->getContinuousHealingMax() * StatsSystem::getStatsSystem()->complexGetSpellEffectElementModifier(player->getLevel(), player->getModifiedSpellEffectElementModifierPoints(curSpell->getContinuousElementType()), player->getLevel()));
			return ss.str();
		}
		break;
	}
	return "";
}

std::string Tooltip::parseInfoText(SpellActionBase *spell, const std::string infoText) const {
	std::string toReturn = infoText;
	std::vector<std::string> stats;
	stats.push_back("%minWeaponDamage%");
	stats.push_back("%maxWeaponDamage%");
	stats.push_back("%minSpellDirectDamage%");
	stats.push_back("%maxSpellDirectDamage%");
	stats.push_back("%minSpellContinuousDamage%");
	stats.push_back("%maxSpellContinuousDamage%");
	stats.push_back("%minDirectHealing%");
	stats.push_back("%maxDirectHealing%");
	stats.push_back("%minContinuousHealing%");
	stats.push_back("%maxContinuousHealing%");

	for (size_t index = 0; index < stats.size(); index++) {
		if (toReturn.find(stats[index]) != toReturn.npos) {
			int findRet = toReturn.find(stats[index]);
			toReturn.replace(findRet, stats[index].length(), getDynamicValues(spell, index));
		}
	}
	return toReturn;
}

/////////////////////////////////////////////////////////////////////////////////////////////
SpellTooltip::SpellTooltip(SpellActionBase *parent_, Player *player_) : parent(parent_) {
	player = player_;
	blockWidth = 32;
	blockHeight = 32;
	smallTooltip = false;
	getParentText();
}

void SpellTooltip::draw(int x, int y) {
	if (tooltipText.empty()) {
		return;
	}

	// check to see if the ticket we got from the player is the same ticket as the player is offering.
	// if not, we reload our tooltip.
	if (player->m_reloadSpellTooltip) {
		player->m_reloadSpellTooltip = false;
		reloadTooltip();
	}
	
	// make sure the tooltip doesnt go "off screen"
	if (x + (curBlockNumberWidth + 2) * blockWidth > ViewPort::Get().getWidth()) {
		x = ViewPort::Get().getWidth() - (curBlockNumberWidth + 2) * blockWidth;
	}

	if (y + (curBlockNumberHeight + 2) * blockHeight > ViewPort::Get().getHeight()) {
		y = ViewPort::Get().getHeight() - (curBlockNumberHeight + 2) * blockHeight;
	}

	// set the first font Y-position on the top of the first tooltip block excluding topborder
	// (we could also center the text in the tooltip, but topaligned is probably bestlooking
	int font_y = y + blockHeight + (curBlockNumberHeight)* blockHeight - toplineHeight;

	DialogCanvas::DrawCanvas(x, y, curBlockNumberWidth, curBlockNumberHeight, blockWidth, blockHeight, false, true);
	// loop through the text vector and print all the text.
	for (unsigned int i = 0; i < tooltipText.size(); i++) {

		Fontrenderer::Get().addText(*tooltipText[i].charset, x + blockWidth , font_y, tooltipText[i].text, tooltipText[i].color, false);
		font_y -= tooltipText[i].charset->lineHeight + 11;
		if (smallTooltip) {
			break;
		}
	}
}

unsigned long SpellTooltip::getNumBitsToUse(unsigned long maxBitValue) {
	return log(maxBitValue) / log(2);
}

void SpellTooltip::getParentText() {
	// remember what level we generated this tooltip
	Vector4f white = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4f grey = Vector4f(0.7f, 0.7f, 0.7f, 1.0f);
	Vector4f blue = Vector4f(0.3f, 0.3f, 1.0f, 1.0f);
	Vector4f green = Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
	Vector4f yellow = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);

	// name of the spell and rank
	addTooltipText(white, &Globals::fontManager.get("verdana_14"), parent->getName());
	addTooltipText(white, &Globals::fontManager.get("verdana_12"), "");
	addTooltipText(grey, &Globals::fontManager.get("verdana_12"), "Rank %d", parent->getRank());

	// display mana or fatigue-cost, if any.
	if (parent->getSpellCost() > 0) {
		if (player->getArchType() == Enums::CharacterArchType::Caster) {
			addTooltipText(blue, &Globals::fontManager.get("verdana_12"), "Mana: %d", parent->getSpellCost());
		}else if (player->getArchType() == Enums::CharacterArchType::Fighter) {
			addTooltipText(yellow, &Globals::fontManager.get("verdana_12"), "Fatigue: %d", parent->getSpellCost());
		}
	}

	/// display required weapons
	if (parent->getRequiredWeapons() != 0) {
		int numberOfRequiredWeapons = 0;
		std::string reqWeaponString = "Requires: ";

		for (size_t curWeaponType = 0; curWeaponType < getNumBitsToUse(Enums::WeaponType::COUNTWT); curWeaponType++) {
			if (parent->getRequiredWeapons() & static_cast<Enums::WeaponType>(1 << curWeaponType)) {
				if (numberOfRequiredWeapons > 0) { reqWeaponString.append(", "); }
				reqWeaponString.append(WeaponType::getWeaponTypeText(static_cast<Enums::WeaponType>(1 << curWeaponType)));
				numberOfRequiredWeapons++;
			}
		}
		if (numberOfRequiredWeapons > 1) {
			reqWeaponString.replace(reqWeaponString.find_last_of(","), 1, " or");
		}
		addTooltipText(blue, &Globals::fontManager.get("verdana_12"), reqWeaponString);
	}

	// display duration if we have any
	if (parent->getDuration() > 0) {
		addTooltipText(white, &Globals::fontManager.get("verdana_12"), "Duration: %s", Utils::ConvertTime(parent->getDuration()).c_str());
	}

	if (parent->getCooldown() > 0) {
		addTooltipText(white, &Globals::fontManager.get("verdana_12"), "Cooldown: %s", Utils::ConvertTime(parent->getCooldown()).c_str());
	}

	// display cast time
	if (parent->getCastTime() == 0){
		addTooltipText(white, &Globals::fontManager.get("verdana_12"), "Casttime: Instant");
	}else {
		addTooltipText(white, &Globals::fontManager.get("verdana_12"), "Casttime: %.2f sec", static_cast<float>(parent->getCastTime()) / 1000);
	}

	// display description.
	addTooltipText(white, &Globals::fontManager.get("verdana_12"), ""); // newline
	addTooltipText(green, &Globals::fontManager.get("verdana_12"), parseInfoText(parent, parent->getInfo()));
}

SpellActionBase* SpellTooltip::getParent() const {
	return parent;
}
/////////////////////////////////////////////////////////////////////////////////////////////
std::vector<TextureRect> ItemTooltip::Textures;

ItemTooltip::ItemTooltip(Item *parent, InventoryItem *inventoryItem) : parent(parent), inventoryItem(inventoryItem) {
	player = &Player::Get();
	blockWidth = 32;
	blockHeight = 32;
	blockNumberWidth = 1;
	blockNumberHeight = 1;
	smallTooltip = false;
	isShopItem = false;
	getParentText();

}

void ItemTooltip::Init(std::vector<TextureRect> textures) {
	Textures = textures;
}

void ItemTooltip::setShopItem(bool isShopItem_) {
	if (isShopItem != isShopItem_) {
		isShopItem = isShopItem_;
		reloadTooltip();
	}
}

void ItemTooltip::draw(int x, int y) {
	if (tooltipText.empty()) {
		return;
	}
	
	if (player->m_reloadItemTooltip) {
		player->m_reloadItemTooltip = false;
		reloadTooltip();	
	}
	
	// we also check to see if the bound spell has changed the displayed cooldown.
	if (parent->getSpell() != NULL) {
		if (player->isSpellOnCooldown(parent->getSpell()->getName()) == true || !currentCooldownText.empty()) {
			if (currentCooldownText != Utils::ConvertTime(player->getTicksOnCooldownSpell(parent->getSpell()->getName()), parent->getSpell()->getCooldown()).c_str()) {
				reloadTooltip();
			}
		}
	}

	// make sure the tooltip doesnt go "off screen"
	if (x + (curBlockNumberWidth + 2) * blockWidth > ViewPort::Get().getWidth()) {
		x = ViewPort::Get().getWidth() - (curBlockNumberWidth + 2) * blockWidth;
	}

	if (y + (curBlockNumberHeight + 2) * blockHeight > ViewPort::Get().getHeight()) {
		y = ViewPort::Get().getHeight() - (curBlockNumberHeight + 2) * blockHeight;
	}
	// set the first font Y-position on the top of the first tooltip block excluding topborder
	// (we could also center the text in the tooltip, but topaligned is probably bestlooking
	int font_y = y + blockHeight + (curBlockNumberHeight)* blockHeight - toplineHeight;

	DialogCanvas::DrawCanvas(x, y, curBlockNumberWidth, curBlockNumberHeight, blockWidth, blockHeight, false, true);

	// loop through the text vector and print all the text.
	for (unsigned int i = 0; i < tooltipText.size(); i++) {

		if (tooltipText[i].text.find("price:") != tooltipText[i].text.npos) {
			drawCoinsLine(x + blockWidth, blockWidth*curBlockNumberWidth - 10, font_y, &tooltipText[i]);
		}else {
			Fontrenderer::Get().addText(*tooltipText[i].charset, x + blockWidth, font_y, tooltipText[i].text, tooltipText[i].color, false);
		}
		font_y -= tooltipText[i].charset->lineHeight + 11;
		if (smallTooltip) {
			break;
		}
	}
	//TextureManager::DrawBuffer();
}

void ItemTooltip::drawCoinsLine(int x, int frameWidth, int y, sTooltipText *tooltipText) {
	std::string realString = tooltipText->text.substr(0, tooltipText->text.find_first_of(":") + 1);

	int stringWidth = tooltipText->charset->getWidth(realString);
	int xoffset = 0;
	for (size_t i = 0; i < 3; i++) {
		if (itemValue[i] != "0") {
			DrawCoin(x + frameWidth - xoffset, y + 1, i);
			int stringWidth = tooltipText->charset->getWidth(itemValue[i]);
			Fontrenderer::Get().addText(*tooltipText->charset, x + frameWidth - xoffset - stringWidth, y, itemValue[i], Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
			xoffset = xoffset + 25 + stringWidth;
		}
	}
	Fontrenderer::Get().addText(*tooltipText->charset, x + frameWidth - xoffset + 20 - stringWidth, y, realString, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
}

void ItemTooltip::addTooltipTextForPercentageAttribute(std::string attributeName, double attributePercentage) {
	
	Vector4f red = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
	Vector4f green = Vector4f(0.0f, 1.0f, 0.0f, 1.0f);

	if (attributePercentage > 0) {
		addTooltipText(green, &Globals::fontManager.get("verdana_12"), "+%.2f%% %s", attributePercentage, attributeName.c_str());
	}else {
		addTooltipText(red, &Globals::fontManager.get("verdana_12"), "%.2f%% %s", attributePercentage, attributeName.c_str());
	}
}

void ItemTooltip::getParentText() {
	// remember what level we generated this tooltip
	shoppingState = player->isShopping();

	Vector4f grey = Vector4f(0.5f, 0.5f, 0.5f, 1.0f);
	Vector4f white = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4f yellow = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
	Vector4f orange = Vector4f(1.0f, 0.5f, 0.0f, 1.0f);
	Vector4f red = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
	Vector4f green = Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
	Vector4f brownish = Vector4f(0.7f, 0.7f, 0.0f, 1.0f);
	
	std::string attribute_string[] = { "armor", "dexterity", "intellect", "strength", "vitality", "wisdom", "health", "mana", "fatigue", "health regen / s", "mana regen / s", "fatigue regen / s" };
	int8_t attribute_values[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	// setting the title of the tooltip based on item quality
	switch (parent->getItemQuality()) {
	case Enums::ItemQuality::POOR:
			addTooltipText(grey, &Globals::fontManager.get("verdana_14"), parent->getName());
			break;
		case Enums::ItemQuality::NORMAL:
			addTooltipText(white, &Globals::fontManager.get("verdana_14"), parent->getName());
			break;
		case Enums::ItemQuality::ENHANCED:
			addTooltipText(yellow, &Globals::fontManager.get("verdana_14"), parent->getName());
			break;
		case Enums::ItemQuality::RARE:
			addTooltipText(orange, &Globals::fontManager.get("verdana_14"), parent->getName());
			break;
		case Enums::ItemQuality::LORE:
			addTooltipText(red, &Globals::fontManager.get("verdana_14"), parent->getName());
			break;
	}

	// add an extra linefeed after the title
	addTooltipText(white, &Globals::fontManager.get("verdana_14"), "");

	// displaying which type of item it is.
	switch (parent->getItemType()) {
		case Enums::ItemType::QUESTITEM:
			addTooltipText(white, &Globals::fontManager.get("verdana_12"), "Quest item");
			break;
		case Enums::ItemType::MISCELLANEOUS:
			// nothing here so far...
			break;
		case Enums::ItemType::ARMOR:
			if (player->canWearArmorType(parent) == true) {
				addTooltipText(white, &Globals::fontManager.get("verdana_12"), ArmorType::getArmorTypeText(parent->getArmorType()));
			}else {
				addTooltipText(red, &Globals::fontManager.get("verdana_12"), ArmorType::getArmorTypeText(parent->getArmorType()));
			}
			break;
		case Enums::ItemType::WEAPON:
			addTooltipText(white, &Globals::fontManager.get("verdana_12"), WeaponType::getWeaponTypeText(parent->getWeaponType()));
			break;
		case Enums::ItemType::JEWELRY:
			// nothing here so far...
			break;
		case Enums::ItemType::DRINK:
			// nothing here so far...
			break;
		case Enums::ItemType::FOOD:
			// nothing here so far...
			break;
		case Enums::ItemType::POTION:
			addTooltipText(white, &Globals::fontManager.get("verdana_12"), "Potion");
			break;
		case Enums::ItemType::SCROLL:
			addTooltipText(white, &Globals::fontManager.get("verdana_12"), "Scroll");
			break;
		case Enums::ItemType::NEWSPELL:
			// nothing here so far...
			break;
		case Enums::ItemType::COUNTIT:
			break;
	}

	// displaying where the item fits.
	if (parent->getEquipPosition() != EquipPosition::NONE) {
		addTooltipText(white, &Globals::fontManager.get("verdana_12"), EquipPosition::getEquipPositionText(parent->getEquipPosition()));
	}

	// displaying damage if it's a weapon.
	if (parent->getItemType() == Enums::ItemType::WEAPON && parent->getWeaponType() != Enums::WeaponType::SHIELD && parent->getWeaponType() != Enums::WeaponType::SPELLBOOK) {
		addTooltipText(white, &Globals::fontManager.get("verdana_12"), "Damage: %d-%d", parent->getMinDamage(), parent->getMaxDamage());
	}

	// display stats given from the item
	attribute_values[0] = parent->getStats(Enums::StatsType::Armor);
	attribute_values[1] = parent->getStats(Enums::StatsType::Dexterity);
	attribute_values[2] = parent->getStats(Enums::StatsType::Intellect);
	attribute_values[3] = parent->getStats(Enums::StatsType::Strength);
	attribute_values[4] = parent->getStats(Enums::StatsType::Vitality);
	attribute_values[5] = parent->getStats(Enums::StatsType::Wisdom);
	attribute_values[6] = parent->getStats(Enums::StatsType::Health);
	attribute_values[7] = parent->getStats(Enums::StatsType::Mana);
	attribute_values[8] = parent->getStats(Enums::StatsType::Fatigue);
	attribute_values[9] = parent->getStats(Enums::StatsType::HealthRegen);
	attribute_values[10] = parent->getStats(Enums::StatsType::ManaRegen);
	attribute_values[11] = parent->getStats(Enums::StatsType::FatigueRegen);

	for (unsigned int i = 0; i < 12; i++) {
		if (attribute_values[i] != 0) {
			if (attribute_values[i] > 0) {
				addTooltipText(green, &Globals::fontManager.get("verdana_12"), "+%d %s", attribute_values[i], attribute_string[i].c_str());

			}else {
				addTooltipText(red, &Globals::fontManager.get("verdana_12"), "%d %s", attribute_values[i], attribute_string[i].c_str());
			}
		}
	}

	int16_t damageModifier = parent->getStats(Enums::StatsType::DamageModifier);
	if (damageModifier != 0) {
		double damageBonus = (StatsSystem::getStatsSystem()->complexGetDamageModifier(player->getLevel(), damageModifier, player->getLevel()) - 1) * 100;
		addTooltipTextForPercentageAttribute("damage bonus", damageBonus);
	}

	int16_t hitModifier = parent->getStats(Enums::StatsType::HitModifier);
	if (hitModifier != 0) {
		double hitBonus = (StatsSystem::getStatsSystem()->complexGetHitChance(player->getLevel(), hitModifier, player->getLevel())) * 100;
		addTooltipTextForPercentageAttribute("chance to hit", hitBonus);
	}

	int16_t evadeModifier = parent->getStats(Enums::StatsType::EvadeModifier);
	if (evadeModifier != 0) {
		double evadeBonus = (StatsSystem::getStatsSystem()->complexGetEvadeChance(player->getLevel(), evadeModifier, player->getLevel())) * 100;
		addTooltipTextForPercentageAttribute("chance to evade", evadeBonus);
	}

	int16_t parryModifier = parent->getStats(Enums::StatsType::ParryModifier);
	if (parryModifier != 0) {
		double parryBonus = (StatsSystem::getStatsSystem()->complexGetParryChance(player->getLevel(), parryModifier, player->getLevel())) * 100;
		addTooltipTextForPercentageAttribute("chance to parry", parryBonus);
	}

	int16_t blockModifier = parent->getStats(Enums::StatsType::BlockModifier);
	if (blockModifier != 0) {
		double blockBonus = (StatsSystem::getStatsSystem()->complexGetEvadeChance(player->getLevel(), blockModifier, player->getLevel())) * 100;
		addTooltipTextForPercentageAttribute("chance to block", blockBonus);
	}

	int16_t meleeCriticalModifier = parent->getStats(Enums::StatsType::MeleeCritical);
	if (meleeCriticalModifier != 0) {
		double meleeCriticalBonus = (StatsSystem::getStatsSystem()->complexGetMeleeCriticalStrikeChance(player->getLevel(), meleeCriticalModifier, player->getLevel())) * 100;
		addTooltipTextForPercentageAttribute("melee critical chance", meleeCriticalBonus);
	}

	for (size_t curElement = 0; curElement< static_cast<size_t>(Enums::ElementType::CountET); ++curElement) {
		Enums::ElementType curElementType = static_cast<Enums::ElementType>(curElement);
		int16_t resistElementModifierPoints = parent->getResistElementModifierPoints(curElementType) + parent->getStats(Enums::StatsType::ResistAll);
		if (resistElementModifierPoints != 0) {
			double resistBonus = (StatsSystem::getStatsSystem()->complexGetResistElementChance(player->getLevel(), resistElementModifierPoints, player->getLevel())) * 100;
			addTooltipTextForPercentageAttribute(std::string("resist ").append(Character::ElementToLowerString(curElementType)), resistBonus);
		}
	}

	for (size_t curElement = 0; curElement< static_cast<size_t>(Enums::ElementType::CountET); ++curElement) {
		Enums::ElementType curElementType = static_cast<Enums::ElementType>(curElement);
		int16_t spellEffectElementModifierPoints = parent->getSpellEffectElementModifierPoints(curElementType) + parent->getStats(Enums::StatsType::SpellEffectAll);
		if (spellEffectElementModifierPoints != 0) {
			double spellEffectBonus = (StatsSystem::getStatsSystem()->complexGetSpellEffectElementModifier(player->getLevel(), spellEffectElementModifierPoints, player->getLevel()) - 1.00) * 100;
			addTooltipTextForPercentageAttribute(Character::ElementToLowerString(curElementType).append(" magic effect"), spellEffectBonus);
		}
	}

	int16_t spellCriticalModifier = parent->getStats(Enums::StatsType::SpellCritical);
	if (spellCriticalModifier != 0) {
		double spellCriticalBonus = (StatsSystem::getStatsSystem()->complexGetSpellCriticalStrikeChance(player->getLevel(), spellCriticalModifier, player->getLevel())) * 100;
		addTooltipTextForPercentageAttribute("spell critical chance", spellCriticalBonus);
	}

	// if the item is useable, display it here and what effect it has.
	if (parent->isUseable()) {
		addTooltipText(green, &Globals::fontManager.get("verdana_11"), parseInfoText(parent->getSpell(), parent->getUseableDescription()));
		if (parent->getSpell() != NULL) {
			if (player->isSpellOnCooldown(parent->getSpell()->getName()) == true) {
				currentCooldownText = Utils::ConvertTime(player->getTicksOnCooldownSpell(parent->getSpell()->getName()), parent->getSpell()->getCooldown());
				addTooltipText(red, &Globals::fontManager.get("verdana_11"), "Cooldown: %s", currentCooldownText.c_str());
			}
		}
	}

	/// if the item has any spell trigger effects, we should inform the player of them here.
	std::vector<TriggerSpellOnItem*> triggerSpells = parent->getTriggerSpells();
	for (size_t curSpell = 0; curSpell < triggerSpells.size(); curSpell++) {
		addTooltipText(orange, &Globals::fontManager.get("verdana_12"), "%.0f%% %s", triggerSpells[curSpell]->getChanceToTrigger() * 100, triggerSpells[curSpell]->getTooltipText().c_str());
	}


	// display the item description, if any
	if (!parent->getDescription().empty()) {
		addTooltipText(brownish, &Globals::fontManager.get("verdana_10"), parent->getDescription());
	}

	// display item level requirements if player's level is too low.
	uint8_t requiredLevel = 1;
	if (parent->isUseable()) {
		if (parent->getSpell()->getRequiredLevel() > parent->getRequiredLevel()) {
			requiredLevel = parent->getSpell()->getRequiredLevel();
		}
		else {
			requiredLevel = parent->getRequiredLevel();
		}
	} else {
		requiredLevel = parent->getRequiredLevel();
	}

	if (requiredLevel > player->getLevel()) {
		addTooltipText(white, &Globals::fontManager.get("verdana_12"), "");
		addTooltipText(red, &Globals::fontManager.get("verdana_12"), "Requires level %d", requiredLevel);
	}

	// display item level requirements if player's level is too low.
	if (parent->isUseable()) {
		if (parent->getSpell()->getRequiredClass() != player->getClass() && parent->getSpell()->getRequiredClass() != Enums::CharacterClass::ANYCLASS) {
			addTooltipText(red, &Globals::fontManager.get("verdana_12"), "Requires class %s", Character::GetCharacterClassName(parent->getSpell()->getRequiredClass()).c_str());
		}

		if (parent->getItemType() == Enums::ItemType::NEWSPELL) {
			if (player->isSpellInscribedInSpellbook(parent->getSpell()) == true) {
				addTooltipText(red, &Globals::fontManager.get("verdana_11"), "Already inscribed in spellbook.");
			}
		}
	}

	int32_t coinsBuyPrice = parent->getValue() * inventoryItem->getCurrentStackSize();
	int32_t coinsSellPrice = floor(parent->getValue() * 0.75) * inventoryItem->getCurrentStackSize();

	if (player->isShopping() && parent->getValue() != 0) {
		if (isShopItem) {	
			itemValue[0] = currency::convertCoinsToString(currency::COPPER, coinsBuyPrice);
			itemValue[1] = currency::convertCoinsToString(currency::SILVER, coinsBuyPrice);
			itemValue[2] = currency::convertCoinsToString(currency::GOLD, coinsBuyPrice);
			addTooltipText(white, &Globals::fontManager.get("verdana_12"), "Buy price: xxxxxxxxxxxx");
		}else {
			itemValue[0] = currency::convertCoinsToString(currency::COPPER, coinsSellPrice);
			itemValue[1] = currency::convertCoinsToString(currency::SILVER, coinsSellPrice);
			itemValue[2] = currency::convertCoinsToString(currency::GOLD, coinsSellPrice);
			addTooltipText(white, &Globals::fontManager.get("verdana_12"), "Sell price: xxxxxxxxxxx");
		}
	}
}

Item* ItemTooltip::getParent() const {
	return parent;
}

void ItemTooltip::DrawCoin(int x, int y, int coin, bool updateView) {

	if (coin == currency::GOLD) {
		TextureManager::DrawTextureBatched(Textures[0], x, y, 16, 16, false, updateView);
	}

	if (coin == currency::SILVER) {
		TextureManager::DrawTextureBatched(Textures[1], x, y, 16, 16, false, updateView);
	}

	if (coin == currency::COPPER) {
		TextureManager::DrawTextureBatched(Textures[2], x, y, 16, 16, false, updateView);
	}
}
