#pragma once

#include <cstdint>
#include <utility>
#include <unordered_map>
#include <iostream>

#include "Enums.h"
#include "TextureManager.h"
#include "Random.h"
#include "SpellActionBase.h"

class TileSet;

struct pair_hash {
	template <class T1, class T2>
	std::size_t operator () (const std::pair<T1, T2> &p) const {
		return std::hash<T1>{}(p.first) ^ std::hash<T2>{}(p.second);
	}
};

struct CharacterType {
	friend class Character;
	friend class Player;
	friend class Npc;

	void addMoveTexture(Enums::ActivityType activity, Enums::Direction direction, int index, std::string filename, unsigned int maxWidth = 0, unsigned int maxHeight = 0, bool reload = false, int paddingLeft = 0, int paddingRight = 0, int paddingTop = 0, int paddingBottom = 0);
	void calcNumMoveTexturesPerDirection();

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
	std::unordered_map<Enums::ActivityType, unsigned short> m_numMoveTexturesPerDirection;

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

	Character();
	~Character() = default;

	virtual void draw() = 0;
	virtual void update(float deltaTime);

	virtual int getWidth() const = 0;
	virtual int getHeight() const = 0;
	virtual unsigned short getNumActivityTextures(Enums::ActivityType activity) = 0;


	void setNumActivities(unsigned short numActivities);
	void baseOnType(std::string otherName);

	Enums::ActivityType getCurActivity() const;
	
	int getXPos() const;
	int getYPos() const;
	
	unsigned short getNumActivities();
	

	Enums::Direction GetOppositeDirection(Enums::Direction direction);
	uint16_t getWanderRadius() const;
	uint16_t getWanderRadiusSq() const;
	float getMovementSpeed() const;
	void giveCoins(unsigned int amountOfCoins);
	bool CheckMouseOver(int _x_pos, int _y_pos);
	bool hasTarget(Character* target);

	void setTarget(Character *target);
	void setTarget(Character *target, Enums::Attitude attitude);
	Character* getTarget() const;

	Character* target;
	Enums::Attitude targetAttitude;

	bool isStunned() const;
	bool isMesmerized() const;
	bool isFeared() const;
	bool isConfused() const;
	bool isCharmed() const;
	bool getIsPreparing() const;
	bool mayDoAnythingAffectingSpellActionWithoutAborting() const;
	bool mayDoAnythingAffectingSpellActionWithAborting() const;
	bool isPlayer() const;
	bool isChanneling() const;
	bool isSneaking() const;

	
	bool continuePreparing();
	void MoveUp(uint8_t n);
	void MoveDown(uint8_t n);
	void MoveLeft(uint8_t n);
	void MoveRight(uint8_t n);
	void Move(char x, char y);
	void Move(Enums::Direction direction, uint8_t n = 1);

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
	void setClass(Enums::CharacterClass characterClass);
	void setExperienceValue(uint8_t experienceValue);
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
	uint32_t coins;
	uint16_t getCurrentHealth() const;
	void modifyCurrentHealth(int16_t currentHealthModifier);
	void setCurrentHealth(uint16_t newCurrentHealth);
	uint16_t getModifiedMaxHealth() const;
	float getPreparationPercentage() const;
	std::string getCurrentSpellActionName() const;

	int boundingBoxX;
	int boundingBoxY;
	int boundingBoxW;
	int boundingBoxH;
	bool useBoundingBox = false;

	void setBoundingBox(int bbx, int bby, int bbw, int bbh);
	void setUseBoundingBox(bool use);
	int getBoundingBoxX() const;
	int getBoundingBoxY() const;
	int getBoundingBoxW() const;
	int getBoundingBoxH() const;
	bool getUseBoundingBox() const ;
	

	void interruptCurrentActivityWith(Enums::ActivityType activity);

	static std::string AttitudeToString(Enums::Attitude attitude);
	static std::string ActivityToString(Enums::ActivityType activity);

	bool alive;
	//bool hasDrawnDyingOnce;
	int current_texture, direction_texture;
	bool hasChoosenFearDirection;
	bool isPreparing = false;
	

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
	//unsigned int remainingMovePoints;


	int x_pos, y_pos;
	Enums::Direction activeDirection;
	Enums::Direction lastActiveDirection;
	Enums::ActivityType curActivity;

	Enums::Direction fearDirection, dyingDirection;
	unsigned short m_numActivities;

	const TextureRect* rect;
	unsigned short index = 0;
	unsigned short currentFrame = 0;
	float m_elapsedTime = 0.0f;
	float m_wanderTime = 0.0f;
	
	bool m_waitForAnimation = false;

	float progress = 0.0f;

	std::string name;
	uint16_t strength;
	uint16_t dexterity;
	uint16_t vitality;
	uint16_t intellect;
	uint16_t wisdom;
	uint16_t max_health;
	uint16_t current_health;
	uint16_t max_mana;
	uint16_t current_mana;
	uint16_t max_fatigue;
	uint16_t current_fatigue;
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
	Enums::CharacterArchType characterArchType;
	float preparationPercentage;
	uint64_t experience;

	CSpellActionBase* curSpellAction;
	std::vector<CSpellActionBase*> spellbook;

	Enums::CharacterArchType getArchType() const;
	uint16_t getCurrentFatigue() const;
	void setCurrentFatigue(uint16_t newCurrentFatigue);
	void modifyCurrentFatigue(int16_t currentFatigueModifier);
	void modifyMaxFatigue(int16_t maxFatigueModifier);
	uint16_t getModifiedMaxFatigue() const;
	void addCooldownSpell(CSpellActionBase *spell);

	void modifyCurrentMana(int16_t currentManaModifier);
	void setCurrentMana(uint16_t newCurrentMana);
	uint16_t getCurrentMana() const;
	void modifyMaxMana(int16_t maxManaModifier);
	uint16_t getModifiedMaxMana() const;
	bool isAlive() const;
	void addActiveSpell(CSpellActionBase *spell);
	bool canBeDamaged() const;
	std::vector<std::pair<CSpellActionBase*, uint32_t>> activeSpells;
	std::vector<std::pair<CSpellActionBase*, uint32_t> > cooldownSpells;
	void Damage(int amount, bool criticalHit);
	void removeSpellsWithCharacterState(Enums::CharacterStates characterState);
	bool isInvisible() const;
	void Die();
	void gainExperience(uint64_t addExp);
	bool canRaiseLevel() const;
	void raiseLevel();
	uint64_t getExpNeededForLevel(uint8_t level) const;
	void executeSpellWithoutCasting(CSpellActionBase *spell, Character *target);
	void Heal(int amount);
	void inscribeSpellInSpellbook(CSpellActionBase *spell);
	std::vector<CSpellActionBase*> getSpellbook() const;
};