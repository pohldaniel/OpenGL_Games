#pragma once
#include <vector>
#include <string>
#include "engine/Rect.h"
#include "Enums.h"

class SpellActionBase;
class Spell;

namespace ArmorType {
	

	inline Enums::ArmorType getHighestArmorTypeByClass(Enums::CharacterClass characterClass) {
		switch (characterClass) {
			case Enums::CharacterClass::Liche:
				return Enums::ArmorType::CLOTH;
				break;
			case Enums::CharacterClass::Ranger:
				return Enums::ArmorType::LEATHER;
				break;
			case Enums::CharacterClass::Warrior:
				return Enums::ArmorType::PLATE;
				break;
			case Enums::CharacterClass::NOCLASS:
				return Enums::ArmorType::NO_ARMOR;
				break;
			case Enums::CharacterClass::ANYCLASS:
				return Enums::ArmorType::NO_ARMOR;
				break;
		}
	}

	inline std::string getArmorTypeText(Enums::ArmorType armorType) {
		switch (armorType) {
			case Enums::ArmorType::CLOTH:
				return "Cloth";
				break;
			case Enums::ArmorType::LEATHER:
				return "Leather";
				break;
			case Enums::ArmorType::MAIL:
				return "Mail";
				break;
			case Enums::ArmorType::PLATE:
				return "Plate";
				break;
			case Enums::ArmorType::NO_ARMOR:
				return "No armor";
				break;
			default:
				return "";
		}
	}
}

namespace WeaponType {
	

	inline std::string getWeaponTypeText(Enums::WeaponType weaponType) {
		switch (weaponType) {
			case Enums::WeaponType::ONEHAND_SWORD:
				return "One handed sword";
				break;
			case Enums::WeaponType::TWOHAND_SWORD:
				return "Two handed sword";
				break;
			case Enums::WeaponType::DAGGER:
				return "Dagger";
				break;
			case Enums::WeaponType::STAFF:
				return "Staff";
				break;
			case Enums::WeaponType::ONEHAND_CLUB:
				return "One handed club";
				break;
			case Enums::WeaponType::TWOHAND_CLUB:
				return "Two handed club";
				break;
			case Enums::WeaponType::WAND:
				return "Wand";
				break;
			case Enums::WeaponType::SPELLBOOK:
				return "Spellbook";
				break;
			case Enums::WeaponType::ONEHAND_MACE:
				return "One handed mace";
				break;
			case Enums::WeaponType::TWOHAND_MACE:
				return "Two handed mace";
				break;
			case Enums::WeaponType::ONEHAND_AXE:
				return "One handed axe";
				break;
			case Enums::WeaponType::TWOHAND_AXE:
				return "Two handed axe";
				break;
			case Enums::WeaponType::BOW:
				return "Bow";
				break;
			case Enums::WeaponType::SLINGSHOT:
				return "Slingshot";
				break;
			case Enums::WeaponType::CROSSBOW:
				return "Crossbow";
				break;
			case Enums::WeaponType::SHIELD:
				return "Shield";
				break;
			case Enums::WeaponType::NO_WEAPON:
				break;
			case Enums::WeaponType::COUNTWT:
				return "";
				break;
		}
	}
}

class TriggerSpellOnItem {
public:
	TriggerSpellOnItem(SpellActionBase* spellToTrigger, float chanceToTrigger, Enums::TriggerType triggerType, bool castOnSelf);
	SpellActionBase *getSpellToTrigger() const;
	float getChanceToTrigger() const;
	Enums::TriggerType getTriggerType() const;
	bool getCastOnSelf() const;
	std::string getTooltipText() const;

private:
	SpellActionBase *spellToTrigger;
	float chanceToTrigger;
	Enums::TriggerType triggerType;
	bool castOnSelf;
};

class Item {
public:

	Item(std::string name, size_t sizeX, size_t sizeY, std::string symbolFile,
		Enums::ItemQuality itemQuality,
		EquipPosition::EquipPosition equipPosition,
		Enums::ItemType itemType,
		Enums::ArmorType armorType,
		Enums::WeaponType weaponType,
		bool loadSymbol = true);
	// probably like CCharacter: baseOnType

	std::string getName() const;
	std::string getID() const;
	std::string getDescription() const;
	void setDescription(std::string description);

	bool isUseable() const;
	std::string getUseableDescription() const;

	bool isTwoHandedWeapon() const;

	int16_t getStats(Enums::StatsType statsType) const;
	int16_t getResistElementModifierPoints(Enums::ElementType elementType) const;
	int16_t getSpellEffectElementModifierPoints(Enums::ElementType elementType) const;
	uint8_t getMinDamage() const;
	uint8_t getMaxDamage() const;
	uint32_t getValue() const;
	uint8_t getRequiredLevel() const;
	Spell *getSpell() const;
	
	void setStats(Enums::StatsType statsType, int16_t amount);
	void setResistElementModifierPoints(Enums::ElementType elementType, int16_t resistModifierPoints);
	void setSpellEffectElementModifierPoints(Enums::ElementType elementType, int16_t spellEffectElementModifierPoints);
	void setMinDamage(uint8_t minDamage);
	void setMaxDamage(uint8_t maxDamage);
	void setValue(uint32_t newValue);
	void setRequiredLevel(uint8_t requiredLevel);
	void setSpell(Spell *newSpell);
	void setItemTextureRect(TextureRect& textureRect);

	///\brief This section handles the spells that can be triggered from actions on the item
	void addTriggerSpellOnSelf(SpellActionBase* spellToTrigger, float chanceToTrigger, Enums::TriggerType triggerType);
	void addTriggerSpellOnTarget(SpellActionBase* spellToTrigger, float chanceToTrigger, Enums::TriggerType triggerType);
	std::vector<TriggerSpellOnItem*> getTriggerSpells() const;

	size_t getMaxStackSize() const;
	void setMaxStackSize(size_t maxStackSize);
	bool isItemStackable() const;

	size_t getSizeX() const;
	size_t getSizeY() const;

	Enums::ItemQuality getItemQuality() const;
	Enums::ItemType getItemType() const;
	Enums::ArmorType getArmorType() const;
	Enums::WeaponType getWeaponType() const;
	EquipPosition::EquipPosition getEquipPosition() const;

	virtual TextureRect* getSymbolTexture();

private:
	std::string name;
	std::string description;

	size_t sizeX;
	size_t sizeY;

	Enums::ItemQuality itemQuality;
	Enums::ItemType itemType;
	Enums::ArmorType armorType;
	Enums::WeaponType weaponType;
	EquipPosition::EquipPosition equipPosition;

	std::vector <TriggerSpellOnItem*> triggerSpells;

	bool useableItem;

	int16_t *statsModifier;
	int16_t *resistElementModifier;
	int16_t *spellEffectElementModifier;

	uint8_t minDamage;
	uint8_t maxDamage;

	size_t maxStackSize;

	uint8_t requiredLevel;

	uint32_t value;

	Spell *spell;

protected:
	TextureRect itemSymbol;
};

class GoldHeap : public Item {

public:
	GoldHeap(size_t coins_);
	size_t numCoins() const;

private:
	size_t coins;
};
