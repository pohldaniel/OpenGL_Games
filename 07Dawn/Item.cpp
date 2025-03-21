#include <memory>
#include <cassert>
#include "Item.h"
#include "Spells.h"

TriggerSpellOnItem::TriggerSpellOnItem(SpellActionBase* spellToTrigger, float chanceToTrigger, Enums::TriggerType triggerType, bool castOnSelf) {

	this->spellToTrigger = spellToTrigger;
	this->chanceToTrigger = chanceToTrigger;
	this->triggerType = triggerType;
	this->castOnSelf = castOnSelf;
}

SpellActionBase *TriggerSpellOnItem::getSpellToTrigger() const {
	return spellToTrigger;
}

float TriggerSpellOnItem::getChanceToTrigger() const {
	return chanceToTrigger;
}

Enums::TriggerType TriggerSpellOnItem::getTriggerType() const {
	return triggerType;
}

bool TriggerSpellOnItem::getCastOnSelf() const {
	return castOnSelf;
}

std::string TriggerSpellOnItem::getTooltipText() const {
	std::string toReturn = "chance to cast ";
	toReturn.append(getSpellToTrigger()->getName());
	if (getCastOnSelf() == true) {
		toReturn.append(" on yourself when ");
	}else {
		toReturn.append(" on your target when ");
	}

	if (getTriggerType() == Enums::EXECUTING_ACTION) {
		toReturn.append("casting spells.");
	}else if (getTriggerType() == Enums::TAKING_DAMAGE) {
		toReturn.append("taking damage.");
	}

	return toReturn;
}

Item::Item(std::string name_, size_t sizeX_, size_t sizeY_, std::string symbolFile,
	Enums::ItemQuality itemQuality_,
	EquipPosition::EquipPosition equipPosition_,
	Enums::ItemType itemType_,
	Enums::ArmorType armorType_,
	Enums::WeaponType weaponType_, bool loadSymbol)
	: name(name_),
	sizeX(sizeX_),
	sizeY(sizeY_),
	itemQuality(itemQuality_),
	itemType(itemType_),
	armorType(armorType_),
	weaponType(weaponType_),
	equipPosition(equipPosition_),
	useableItem(false),
	statsModifier(NULL),
	resistElementModifier(NULL),
	spellEffectElementModifier(NULL),
	minDamage(0),
	maxDamage(0),
	maxStackSize(1),
	requiredLevel(1),
	value(0),
	spell(NULL) {

	resistElementModifier = new int16_t[static_cast<size_t>(Enums::ElementType::CountET)];
	spellEffectElementModifier = new int16_t[static_cast<size_t>(Enums::ElementType::CountET)];
	statsModifier = new int16_t[static_cast<size_t>(Enums::StatsType::CountST)];

	for (size_t curElement = 0; curElement<static_cast<size_t>(Enums::ElementType::CountET); ++curElement) {
		resistElementModifier[curElement] = 0;
		spellEffectElementModifier[curElement] = 0;
	}

	for (size_t curStat = 0; curStat<static_cast<size_t>(Enums::StatsType::CountST); ++curStat) {
		statsModifier[curStat] = 0;
	}

	// note: This is for use in derived classes that set their own texture ID only
	if (loadSymbol) {
		//symbolTexture.LoadIMG(symbolFile, 0);
	}

	if (itemType == Enums::ItemType::DRINK ||
		itemType == Enums::ItemType::FOOD ||
		itemType == Enums::ItemType::POTION ||
		itemType == Enums::ItemType::NEWSPELL ||
		itemType == Enums::ItemType::SCROLL) {
		useableItem = true;
	}
}

std::string Item::getName() const {
	return name;
}

std::string Item::getID() const {
	std::ostringstream idstream;
	for (size_t curChar = 0; curChar<name.size(); ++curChar) {
		if (islower(tolower(name[curChar]))) {
			idstream << char(tolower(name[curChar]));
		}
	}

	if (spell != NULL && spell->getRank() > 1) {
		idstream << "rank" << static_cast<int>(spell->getRank());
	}

	return idstream.str();
}

size_t Item::getSizeX() const {
	return sizeX;
}

size_t Item::getSizeY() const {
	return sizeY;
}

size_t Item::getMaxStackSize() const {
	return maxStackSize;
}

void Item::setMaxStackSize(size_t maxStackSize) {
	this->maxStackSize = maxStackSize;
}

bool Item::isItemStackable() const {

	if (getMaxStackSize() > 1) {
		return true;
	}
	return false;
}

TextureRect* Item::getSymbolTexture() {
	return &itemSymbol;
}

Enums::ItemQuality Item::getItemQuality() const {
	return itemQuality;
}

Enums::ItemType Item::getItemType() const {
	return itemType;
}

Enums::ArmorType Item::getArmorType() const {
	return armorType;
}

Enums::WeaponType Item::getWeaponType() const {
	return weaponType;
}

EquipPosition::EquipPosition Item::getEquipPosition() const {
	return equipPosition;
}

std::string Item::getDescription() const {
	return description;
}

int16_t Item::getStats(Enums::StatsType statsType) const {
	return statsModifier[static_cast<size_t>(statsType)];
}

int16_t Item::getResistElementModifierPoints(Enums::ElementType elementType) const {
	return resistElementModifier[static_cast<size_t>(elementType)];
}

int16_t Item::getSpellEffectElementModifierPoints(Enums::ElementType elementType) const {
	return spellEffectElementModifier[static_cast<size_t>(elementType)];
}

uint8_t Item::getMinDamage() const {
	return minDamage;
}

uint8_t Item::getMaxDamage() const {
	return maxDamage;
}

uint8_t Item::getRequiredLevel() const {
	return requiredLevel;
}

uint32_t Item::getValue() const {
	return value;
}

Spell *Item::getSpell() const {
	return spell;
}

void Item::setStats(Enums::StatsType statsType, int16_t amount) {
	this->statsModifier[static_cast<size_t>(statsType)] = amount;
}

void Item::setResistElementModifierPoints(Enums::ElementType elementType, int16_t resistModifierPoints) {
	this->resistElementModifier[static_cast<size_t>(elementType)] = resistModifierPoints;
}

void Item::setSpellEffectElementModifierPoints(Enums::ElementType elementType, int16_t spellEffectElementModifierPoints) {
	this->spellEffectElementModifier[static_cast<size_t>(elementType)] = spellEffectElementModifierPoints;
}

void Item::setMinDamage(uint8_t minDamage_) {
	minDamage = minDamage_;
}

void Item::setMaxDamage(uint8_t maxDamage_) {
	maxDamage = maxDamage_;
}

void Item::setRequiredLevel(uint8_t requiredLevel) {
	this->requiredLevel = requiredLevel;
}

void Item::setValue(uint32_t newValue) {
	value = newValue;
}

void Item::setSpell(Spell *newSpell) {
	spell = newSpell;
}

void Item::setDescription(std::string description_) {
	description = "\"" + description_ + "\"";
}

void Item::addTriggerSpellOnSelf(SpellActionBase* spellToTrigger, float chanceToTrigger, Enums::TriggerType triggerType) {
	triggerSpells.push_back(new TriggerSpellOnItem(spellToTrigger, chanceToTrigger, triggerType, true));
}

void Item::addTriggerSpellOnTarget(SpellActionBase* spellToTrigger, float chanceToTrigger, Enums::TriggerType triggerType) {
	triggerSpells.push_back(new TriggerSpellOnItem(spellToTrigger, chanceToTrigger, triggerType, false));
}

std::vector<TriggerSpellOnItem*> Item::getTriggerSpells() const {
	return triggerSpells;
}

bool Item::isTwoHandedWeapon() const {
	switch (getWeaponType()) {
		case Enums::WeaponType::TWOHAND_AXE:
			return true;
			break;
		case Enums::WeaponType::TWOHAND_CLUB:
			break;
			return true;
		case Enums::WeaponType::TWOHAND_MACE:
			break;
			return true;
		case Enums::WeaponType::TWOHAND_SWORD:
			return true;
			break;
		case Enums::WeaponType::BOW:
			return true;
			break;
		case Enums::WeaponType::CROSSBOW:
			return true;
			break;
		case Enums::WeaponType::STAFF:
			return true;
			break;
		default:
			return false;
			break;
	}

	return false;
}

std::string Item::getUseableDescription() const {
	switch (itemType) {
		case Enums::ItemType::DRINK:
			if (getSpell() != NULL) {
				return std::string("Drink: ").append(getSpell()->getInfo());
			}
			break;
		case Enums::ItemType::FOOD:
			if (getSpell() != NULL) {
				return std::string("Eat: ").append(getSpell()->getInfo());
			}
			break;
		case Enums::ItemType::NEWSPELL:
			if (getSpell() != NULL) {
				char rank[10];
				sprintf(rank, " (rank %d)", getSpell()->getRank());
				return std::string("Memorize: Inscribes ").append(getSpell()->getName()).append(rank).append(" to spellbook.");
			}
			break;
		case Enums::ItemType::POTION:
			if (getSpell() != NULL) {
				return std::string("Quaff: ").append(getSpell()->getInfo());
			}
			break;
		case Enums::ItemType::SCROLL:
			if (getSpell() != NULL) {
				return std::string("Read: ").append(getSpell()->getInfo());
			}
			break;
		default:
			break;
	}
	return "";
}

bool Item::isUseable() const {
	return useableItem;
}

void Item::setItemTextureRect(TextureRect& textureRect) {
	itemSymbol = textureRect;
}

GoldHeap::GoldHeap(size_t coins_)
	: Item("Coins", 1, 1, "res/items/coins.tga",
		Enums::ItemQuality::NORMAL,
		EquipPosition::EquipPosition::NONE,
		Enums::ItemType::MISCELLANEOUS,
		Enums::ArmorType::NO_ARMOR,
		Enums::WeaponType::NO_WEAPON,
		false),
	coins(coins_) {
	//symbolTexture.LoadIMG("data/items/coins.tga", 0);
}

size_t GoldHeap::numCoins() const {
	return coins;
}
