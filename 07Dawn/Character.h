#pragma once

#include <cstdint>
#include <utility>
#include <unordered_map>
#include <iostream>

#include "Enums.h"
#include "TextureManager.h"
#include "Random.h"

class TileSet;

struct pair_hash {
	template <class T1, class T2>
	std::size_t operator () (const std::pair<T1, T2> &p) const {
		return std::hash<T1>{}(p.first) ^ std::hash<T2>{}(p.second);
	}
};

struct CharacterType {
	friend class Character;

	void addMoveTexture(Enums::ActivityType activity, Enums::Direction direction, int index, std::string filename, int textureOffsetX = 0, int textureOffsetY = 0);

	void setStrength(uint16_t newStrength);
	void setDexterity(uint16_t newDexterity);
	void setVitality(uint16_t newVitality);
	void setIntellect(uint16_t newIntellect);
	void setWisdom(uint16_t newWisdom);
	void setMaxHealth(uint16_t newMaxHealth);
	void setMaxMana(uint16_t newMaxMana);
	void setMaxFatigue(uint16_t newMaxFatigue);
	void setHealthRegen(uint16_t newHealthRegen);
	void setManaRegen(uint16_t newManaRegen);
	void setFatigueRegen(uint16_t newFatigueRegen);
	void setMinDamage(uint16_t newMinDamage);
	void setMaxDamage(uint16_t newMaxDamage);
	void setArmor(uint16_t newArmor);
	void setDamageModifierPoints(uint16_t newDamageModifierPoints);
	void setHitModifierPoints(uint16_t newHitModifierPoints);
	void setEvadeModifierPoints(uint16_t newEvadeModifierPoints);
	void setName(std::string newName);
	void setWanderRadius(uint16_t newWanderRadius);
	void setLevel(uint8_t newLevel);
	void setClass(Enums::CharacterClass);
	//void inscribeSpellInSpellbook(CSpellActionBase* spell);
	//void addItemToLootTable(Item* item, double dropChance);
	void setExperienceValue(uint8_t experienceValue);

	const TileSet& getTileSet(Enums::ActivityType activity, Enums::Direction direction) const;
	
private:
	std::unordered_map<std::pair<int, int>, TileSet, pair_hash> m_moveTileSets;

	std::string name;
	uint16_t strength;
	uint16_t dexterity;
	uint16_t vitality;
	uint16_t intellect;
	uint16_t wisdom;
	uint16_t max_health;
	uint16_t max_mana;
	uint16_t max_fatigue;
	uint16_t healthRegen;
	uint16_t manaRegen;
	uint16_t fatigueRegen;
	uint8_t experienceValue;
	uint16_t wander_radius;
	uint16_t min_damage;
	uint16_t max_damage;
	uint8_t level;
	uint16_t armor;
	uint16_t damageModifierPoints;
	uint16_t hitModifierPoints;
	uint16_t evadeModifierPoints;
	Enums::CharacterClass characterClass;
};

class CharacterTypeManager {

public:
	const CharacterType& getCharacterType(std::string characterType);
	bool containsCaracterType(std::string characterType);

	std::unordered_map<std::string, CharacterType>& getCharacterTypes();

	static CharacterTypeManager& Get();

private:
	CharacterTypeManager() = default;
	std::unordered_map<std::string, CharacterType> m_characterTypes;
	static CharacterTypeManager s_instance;
};


class Character{

public:

	Character(const CharacterType& characterType);
	~Character() = default;

	virtual void draw() {}
	void update(float deltaTime);

	
	void setNumActivities(unsigned short numActivities);
	void baseOnType(std::string otherName);

	Enums::ActivityType getCurActivity() const;
	int getXPos() const;
	int getYPos() const;
	
	unsigned short getNumActivities();
	virtual Enums::Direction GetDirection() = 0;

	Enums::Direction GetDirectionTexture();
	uint16_t getWanderRadius() const;
	float getMovementSpeed() const;

	bool isStunned() const;
	bool isMesmerized() const;
	bool isFeared() const;
	bool isCharmed() const;
	bool getIsPreparing() const;
	bool mayDoAnythingAffectingSpellActionWithoutAborting() const;
	bool mayDoAnythingAffectingSpellActionWithAborting() const;
	bool isPlayer() const;
	bool isChanneling() const;
	bool isSneaking() const;

	void Move();
	bool continuePreparing();
	void MoveUp(uint8_t n);
	void MoveDown(uint8_t n);
	void MoveLeft(uint8_t n);
	void MoveRight(uint8_t n);


	/*void setStrength(uint16_t newStrength);
	void setDexterity(uint16_t newDexterity);
	void setVitality(uint16_t newVitality);
	void setIntellect(uint16_t newIntellect);
	void setWisdom(uint16_t newWisdom);
	void setMaxHealth(uint16_t newMaxHealth);
	void setMaxMana(uint16_t newMaxMana);
	void setMaxFatigue(uint16_t newMaxFatigue);
	void setHealthRegen(uint16_t newHealthRegen);
	void setManaRegen(uint16_t newManaRegen);
	void setFatigueRegen(uint16_t newFatigueRegen);
	void setMinDamage(uint16_t newMinDamage);
	void setMaxDamage(uint16_t newMaxDamage);
	void setArmor(uint16_t newArmor);
	void setDamageModifierPoints(uint16_t newDamageModifierPoints);
	void setHitModifierPoints(uint16_t newHitModifierPoints);
	void setEvadeModifierPoints(uint16_t newEvadeModifierPoints);
	void setName(std::string newName);
	void setWanderRadius(uint16_t newWanderRadius);
	void setLevel(uint8_t newLevel);
	void setClass(CharacterClass::CharacterClass);
	void setExperienceValue(uint8_t experienceValue);*/
	//void inscribeSpellInSpellbook(CSpellActionBase* spell);
	//void addItemToLootTable(Item* item, double dropChance);
	
	uint16_t getStrength() const;
	uint16_t getDexterity() const;
	uint16_t getVitality() const;
	uint16_t getIntellect() const;
	uint16_t getWisdom() const;
	uint16_t getMaxHealth() const;
	uint16_t getMaxMana() const;
	uint16_t getMaxFatigue() const;
	uint16_t getMaxDamage() const;
	uint16_t getMinDamage() const;
	uint16_t getArmor() const;
	uint16_t getHealthRegen() const;
	uint16_t getManaRegen() const;
	uint16_t getFatigueRegen() const;
	uint16_t getDamageModifierPoints() const;
	uint16_t getHitModifierPoints() const;
	uint16_t getEvadeModifierPoints() const;
	Enums::CharacterClass getClass() const;
	std::string getName() const;
	uint8_t getLevel() const;
	uint8_t getExperienceValue() const;

	int getWidth() const;
	int getHeight() const;

	bool alive;
	//bool hasDrawnDyingOnce;
	int current_texture, direction_texture;
	bool hasChoosenFearDirection;
	bool isPreparing;
	bool useBoundingBox = false;
	int boundingBoxW = 0;
	int boundingBoxH = 0;

	// timers
	float wander_thisframe, wander_lastframe;
	float respawn_thisframe, respawn_lastframe;
	float dyingStartFrame, reduceDyingTranspFrame;

	int wander_every_seconds, wander_points_left;
	bool do_respawn;
	int x_spawn_pos, y_spawn_pos;
	int NPC_id;
	int seconds_to_respawn;
	bool wandering, moving, in_combat;
	uint32_t remainingMovePoints;


	
	//std::unordered_map<std::pair<int, int>, TileSet, pair_hash> m_moveTileSets;

	int x_pos, y_pos;
	
	Enums::Direction activeDirection;
	Enums::Direction WanderDirection, MovingDirection, fearDirection, dyingDirection;
	unsigned short m_numActivities;

	const TextureRect* rect;
	int index = 0;
	float dumping = 0.0f;

	std::string name;
	uint16_t strength;
	uint16_t dexterity;
	uint16_t vitality;
	uint16_t intellect;
	uint16_t wisdom;
	uint16_t max_health;
	//uint16_t current_health;
	uint16_t max_mana;
	//uint16_t current_mana;
	uint16_t max_fatigue;
	//uint16_t current_fatigue;
	uint16_t healthRegen;
	uint16_t manaRegen;
	uint16_t fatigueRegen;
	uint8_t experienceValue;
	uint16_t wander_radius;
	uint16_t min_damage;
	uint16_t max_damage;
	uint8_t level;
	uint16_t armor;
	uint16_t damageModifierPoints;
	uint16_t hitModifierPoints;
	uint16_t evadeModifierPoints;
	Enums::CharacterClass characterClass;

	const CharacterType& m_characterType;
};