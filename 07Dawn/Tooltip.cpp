#include "Tooltip.h"

#include "Character.h"
#include "Player.h"

#include <memory>


spellTooltip::spellTooltip(CSpellActionBase *parent_, Player *player_) : parent(parent_) {
	player = player_;
	blockWidth = 32;
	blockHeight = 32;
	smallTooltip = false;
	loadTextures();
	getParentText();
}

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

void Tooltip::loadTextures() {
	//Frames::initFrameTextures();
}

void Tooltip::reloadTooltip() {
	getTicketFromPlayer();
	//shoppingState = player->isShopping();
	tooltipText.clear();
	getParentText();
}

int Tooltip::getTooltipWidth() const {
	return blockWidth * curBlockNumberWidth + blockWidth;
}

int Tooltip::getTooltipHeight() const {
	return blockHeight * curBlockNumberHeight + blockHeight;
}

void spellTooltip::getTicketFromPlayer() {
	ticketFromPlayer = player->getTicketForSpellTooltip();
}

void spellTooltip::draw(int x, int y) {
	if (tooltipText.empty()) {
		return;
	}

	// check to see if the ticket we got from the player is the same ticket as the player is offering.
	// if not, we reload our tooltip.
	if (ticketFromPlayer != player->getTicketForSpellTooltip()) {
		reloadTooltip();
	}

	// make sure the tooltip doesnt go "off screen"
	if (x + (curBlockNumberWidth + 2) * blockWidth > ViewPort::get().getWidth()) {
		x = ViewPort::get().getWidth() - (curBlockNumberWidth + 2) * blockWidth;
	}

	if (y + (curBlockNumberHeight + 2) * blockHeight > ViewPort::get().getHeight()) {
		y = ViewPort::get().getHeight() - (curBlockNumberHeight + 2) * blockHeight;
	}

	// set the correct position based on where we are
	x += ViewPort::get().getBottomLeft()[0];
	y += ViewPort::get().getBottomLeft()[1];

	// set the first font Y-position on the top of the first tooltip block excluding topborder
	// (we could also center the text in the tooltip, but topaligned is probably bestlooking
	int font_y = y + blockHeight + (curBlockNumberHeight)* blockHeight - toplineHeight;

	//Frames::drawFrame(x, y, curBlockNumberWidth, curBlockNumberHeight, blockWidth, blockHeight);

	// loop through the text vector and print all the text.
	for (unsigned int i = 0; i < tooltipText.size(); i++) {
		//glColor4fv(tooltipText[i].color);
		//tooltipText[i].font->drawText(x + blockWidth, font_y, tooltipText[i].text);
		//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		//font_y -= tooltipText[i].font->getHeight() + 11;
		if (smallTooltip) {
			break;
		}
	}
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
	}
	else if (str.find("price:") != str.npos) {
		// else check to see if the text contains price information. if so we dont wordwrap.
		tooltipText.push_back(sTooltipText(buf, color, charSet));

	}else {
		// format the text into several lines so that the tooltip doesnt get too wide,
		//then push all the text lines to our vector.
		//GLFT_Font *tempfont = FontCache::getFontFromCache("data/verdana.ttf", fontSize);
		std::vector<std::string> formattedLines;
		//formatMultilineText(buf, formattedLines, 300, tempfont);
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

	for (unsigned int i = 0; i < tooltipText.size(); i++)
	{
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

void spellTooltip::getParentText() {
	// remember what level we generated this tooltip
	ticketFromPlayer = player->getTicketForSpellTooltip();

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
	/*if (parent->getRequiredWeapons() != 0) {
		int numberOfRequiredWeapons = 0;
		std::string reqWeaponString = "Requires: ";

		for (size_t curWeaponType = 0; curWeaponType < getNumBitsToUse(WeaponType::COUNT); curWeaponType++) {
			if (parent->getRequiredWeapons() & static_cast<WeaponType::WeaponType>(1 << curWeaponType)) {
				if (numberOfRequiredWeapons > 0) { reqWeaponString.append(", "); }
				reqWeaponString.append(WeaponType::getWeaponTypeText(static_cast<WeaponType::WeaponType>(1 << curWeaponType)));
				numberOfRequiredWeapons++;
			}
		}
		if (numberOfRequiredWeapons > 1) {
			reqWeaponString.replace(reqWeaponString.find_last_of(","), 1, " or");
		}
		addTooltipText(blue, 12, reqWeaponString);
	}

	// display duration if we have any
	if (parent->getDuration() > 0) {
		addTooltipText(white, 12, "Duration: %s", TimeConverter::convertTime(parent->getDuration()).c_str());
	}

	if (parent->getCooldown() > 0) {
		addTooltipText(white, 12, "Cooldown: %s", TimeConverter::convertTime(parent->getCooldown()).c_str());
	}*/

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

std::string Tooltip::getDynamicValues(CSpellActionBase *spell, size_t val) const {
	//const StatsSystem *statsSystem = StatsSystem::getStatsSystem();
	std::stringstream ss;
	ss.str() = "";

	/*switch (val) {
	case 0: // minWeaponDamage
		if (dynamic_cast<MeleeDamageAction*>(spell) != NULL) {
			MeleeDamageAction *curSpell = dynamic_cast<MeleeDamageAction*>(spell);
			ss << static_cast<int16_t>(player->getModifiedMinDamage() * statsSystem->complexGetDamageModifier(player->getLevel(), player->getModifiedDamageModifierPoints(), player->getLevel()) * curSpell->getDamageBonus());
			return ss.str();
		}
		else if (dynamic_cast<RangedDamageAction*>(spell) != NULL) {
			RangedDamageAction *curSpell = dynamic_cast<RangedDamageAction*>(spell);
			ss << static_cast<int16_t>(player->getModifiedMinDamage() * statsSystem->complexGetDamageModifier(player->getLevel(), player->getModifiedDamageModifierPoints(), player->getLevel()) * curSpell->getDamageBonus());
			return ss.str();
		}
		break;
	case 1: // maxWeaponDamage
		if (dynamic_cast<MeleeDamageAction*>(spell) != NULL) {
			MeleeDamageAction *curSpell = dynamic_cast<MeleeDamageAction*>(spell);
			ss << static_cast<int16_t>(player->getModifiedMaxDamage() * statsSystem->complexGetDamageModifier(player->getLevel(), player->getModifiedDamageModifierPoints(), player->getLevel()) * curSpell->getDamageBonus());
			return ss.str();
		}
		else if (dynamic_cast<RangedDamageAction*>(spell) != NULL) {
			RangedDamageAction *curSpell = dynamic_cast<RangedDamageAction*>(spell);
			ss << static_cast<int16_t>(player->getModifiedMaxDamage() * statsSystem->complexGetDamageModifier(player->getLevel(), player->getModifiedDamageModifierPoints(), player->getLevel()) * curSpell->getDamageBonus());
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
	}*/
	return "";
}

std::string Tooltip::parseInfoText(CSpellActionBase *spell, const std::string infoText) const {
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

CSpellActionBase *spellTooltip::getParent() const {
	return parent;
}

namespace DrawFunctions {
	//std::auto_ptr<CTexture> drawTextures(NULL);

	void initDrawTextures() {
		/*if (drawTextures.get() != NULL) {
			return;
		}

		drawTextures = std::auto_ptr<CTexture>(new CTexture());
		drawTextures->LoadIMG("data/interface/inventory/goldcoin.tga", 0);
		drawTextures->LoadIMG("data/interface/inventory/silvercoin.tga", 1);
		drawTextures->LoadIMG("data/interface/inventory/coppercoin.tga", 2);*/
	}

	void drawCoin(int x, int y, int coin) {
		/*if (coin == currency::GOLD) {
			DrawingHelpers::mapTextureToRect(drawTextures->getTexture(0), x, 16, y, 16);
		}

		if (coin == currency::SILVER) {
			DrawingHelpers::mapTextureToRect(drawTextures->getTexture(1), x, 16, y, 16);
		}

		if (coin == currency::COPPER) {
			DrawingHelpers::mapTextureToRect(drawTextures->getTexture(2), x, 16, y, 16);
		}*/
	}
}
