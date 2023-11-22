#pragma once
namespace Enums {
	enum TileClassificationType {
		FLOOR,
		ENVIRONMENT,
		SHADOW,
		COLLISION,
		NPC,
		COUNT
	};

	enum AdjacencyType {
		RIGHT,
		LEFT,
		TOP,
		BOTTOM
	};

	enum Direction {
		STOP = 0,
		N = 1,
		NE = 2,
		E = 3,
		SE = 4,
		S = 5,
		SW = 6,
		W = 7,
		NW = 8
	};

	enum ActivityType {
		Walking,
		Casting,
		Attacking,
		Shooting,
		Dying,
		Count
	};

	enum Attitude {
		FRIENDLY,
		NEUTRAL,
		HOSTILE
	};

	enum CharacterClass {
		NOCLASS,
		ANYCLASS,
		Liche,
		Ranger,
		Warrior
	};

	enum CharacterArchType {
		Fighter,
		Caster
	};

	enum InteractionType {
		Quest,
		Shop,
		Zone
	};

	enum CharacterStates {
		Channeling,
		Charmed,
		Confused,
		Feared,
		Invisible,
		Mesmerized,
		Movementspeed,
		SeeInvisible,
		SeeSneaking,
		Sneaking,
		Stunned,
		NOEFFECT
	};

	inline bool isStateConsideredHarmfull(CharacterStates characterState, float characterStateValue) {
		switch (characterState) {
			case CharacterStates::Channeling:
				return false;
				break;
			case CharacterStates::Charmed:
				return true;
				break;
			case CharacterStates::Confused:
				return true;
				break;
			case CharacterStates::Feared:
				return true;
				break;
			case CharacterStates::Invisible:
				return false;
				break;
			case CharacterStates::Mesmerized:
				return true;
				break;
			case CharacterStates::Movementspeed:
				if (characterStateValue >= 1.0) {
					return false;
				}else {
					return true;
				}
				break;
			case CharacterStates::SeeInvisible:
				return false;
				break;
			case CharacterStates::SeeSneaking:
				return false;
				break;
			case CharacterStates::Sneaking:
				return false;
				break;
			case CharacterStates::Stunned:
				return true;
				break;
			case CharacterStates::NOEFFECT:
				return false;
				break;
			default:
				return false;
		}
	}

	enum EffectType {
		SingleTargetSpell,
		SelfAffectingSpell,
		AreaTargetSpell
	};

	enum ElementType{
		Fire,
		Water,
		Air,
		Earth,
		Light,
		Dark,
		CountET
	};

	enum StatsType {
		Dexterity,
		Intellect,
		Strength,
		Vitality,
		Wisdom,
		Health,
		Mana,
		Fatigue,
		Armor,
		DamageModifier,
		HitModifier,
		EvadeModifier,
		ParryModifier,
		BlockModifier,
		MeleeCritical,
		SpellCritical,
		ResistAll,
		SpellEffectAll,
		HealthRegen,
		ManaRegen,
		FatigueRegen,
		CountST
	};

	enum ItemSlot {
		HEAD,
		AMULET,
		MAIN_HAND,
		OFF_HAND,
		BELT,
		LEGS,
		SHOULDER,
		CHEST,
		GLOVES,
		CLOAK,
		BOOTS,
		RING_ONE,
		RING_TWO,
		COUNTIS
	};

	enum ItemQuality {
		POOR,
		NORMAL,
		ENHANCED,
		RARE,
		LORE
	};

	
	enum ItemType {
		QUESTITEM,
		MISCELLANEOUS,
		ARMOR,
		WEAPON,
		JEWELRY,
		SCROLL,
		POTION,
		FOOD,
		DRINK,
		NEWSPELL,
		COUNTIT
	};

	enum ArmorType {
		NO_ARMOR,
		CLOTH,
		LEATHER,
		MAIL,
		PLATE
	};

	enum WeaponType {
		NO_WEAPON = 0x01,
		ONEHAND_SWORD = 0x02,
		TWOHAND_SWORD = 0x04,
		DAGGER = 0x08,
		STAFF = 0x10,
		ONEHAND_CLUB = 0x20,
		TWOHAND_CLUB = 0x40,
		WAND = 0x80,
		SPELLBOOK = 0x100, // offhand spellbooks for caster classes.
		ONEHAND_MACE = 0x200,
		TWOHAND_MACE = 0x400,
		ONEHAND_AXE = 0x800,
		TWOHAND_AXE = 0x1000,
		BOW = 0x2000,
		SLINGSHOT = 0x4000,
		CROSSBOW = 0x8000,
		SHIELD = 0x10000,
		COUNTWT = 0x20000
	};

	enum TriggerType {
		TAKING_DAMAGE,
		EXECUTING_ACTION
	};

	enum PositionType {
		BOTTOMLEFT,
		BOTTOMCENTER,
		LEFTCENTER,
		CENTER
	};
}

namespace EquipPosition {
	enum EquipPosition {
		MAIN_HAND,
		OFF_HAND,
		CHEST,
		LEGS,
		BELT,
		BOOTS,
		RING,
		GLOVES,
		HEAD,
		CLOAK,
		AMULET,
		SHOULDER,
		NONE
	};

	inline std::string getEquipPositionText(EquipPosition equipPosition) {
		switch (equipPosition) {
			case EquipPosition::MAIN_HAND:
				return "Main hand";
				break;
			case EquipPosition::OFF_HAND:
				return "Off hand";
				break;
			case EquipPosition::CHEST:
				return "Chest";
				break;
			case EquipPosition::LEGS:
				return "Legs";
				break;
			case EquipPosition::BELT:
				return "Belt";
				break;
			case EquipPosition::BOOTS:
				return "Boots";
				break;
			case EquipPosition::RING:
				return "Ring";
				break;
			case EquipPosition::GLOVES:
				return "Gloves";
				break;
			case EquipPosition::HEAD:
				return "Head";
				break;
			case EquipPosition::CLOAK:
				return "Cloak";
				break;
			case EquipPosition::AMULET:
				return "Amulet";
				break;
			case EquipPosition::SHOULDER:
				return "Shoulder";
				break;
			case EquipPosition::NONE:
				return "Unequippable";
				break;
			default:
				return "";
		}
	}
}
