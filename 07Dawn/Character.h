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
	void baseOnType(std::string name);

	void inscribeSpellInSpellbook(SpellActionBase* spell);
	//void addItemToLootTable(Item* item, double dropChance);
	void setClass(Enums::CharacterClass);
	void setName(std::string newName);

	void setStrength(unsigned short newStrength);
	void setDexterity(unsigned short newDexterity);
	void setVitality(unsigned short newVitality);
	void setIntellect(unsigned short newIntellect);
	void setWisdom(unsigned short newWisdom);
	void setMaxHealth(unsigned short newMaxHealth);
	void setMaxMana(unsigned short newMaxMana);
	void setMaxFatigue(unsigned short newMaxFatigue);
	void setHealthRegen(unsigned short newHealthRegen);
	void setManaRegen(unsigned short newManaRegen);
	void setFatigueRegen(unsigned short newFatigueRegen);
	void setMinDamage(unsigned short newMinDamage);
	void setMaxDamage(unsigned short newMaxDamage);
	void setArmor(unsigned short newArmor);
	void setDamageModifierPoints(unsigned short newDamageModifierPoints);
	void setHitModifierPoints(unsigned short newHitModifierPoints);
	void setEvadeModifierPoints(unsigned short newEvadeModifierPoints);	
	void setWanderRadius(unsigned short newWanderRadius);
	void setLevel(unsigned short newLevel);
	void setExperienceValue(unsigned short experienceValue);

	void modifyStrength(int strengthModifier);
	void modifyDexterity(int dexterityModifier);
	void modifyVitality(int vitalityModifier);
	void modifyIntellect(int intellectModifier);
	void modifyWisdom(int wisdomModifier);
	void modifyMaxHealth(int maxHealthModifier);
	void modifyMaxFatigue(int maxFatigueModifier);

	const TileSet& getTileSet(Enums::ActivityType activity, Enums::Direction direction) const;
	
private:
	std::unordered_map<std::pair<int, int>, TileSet, pair_hash> m_moveTileSets;
	std::unordered_map<Enums::ActivityType, unsigned short> m_numMoveTexturesPerDirection;

	std::string name;
	unsigned short strength;
	unsigned short dexterity;
	unsigned short vitality;
	unsigned short intellect;
	unsigned short wisdom;
	unsigned short max_health;
	unsigned short max_mana;
	unsigned short max_fatigue;
	unsigned short healthRegen;
	unsigned short manaRegen;
	unsigned short fatigueRegen;	
	unsigned short wander_radius;
	unsigned short min_damage;
	unsigned short max_damage;
	unsigned short armor;
	unsigned short damageModifierPoints;
	unsigned short hitModifierPoints;
	unsigned short evadeModifierPoints;
	unsigned short experienceValue;
	unsigned short level;
	Enums::CharacterClass characterClass;

	std::vector<SpellActionBase*> spellbook;
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

	friend class Editor;
	friend class SpellActionBase;

public:

	Character();
	~Character() = default;

	virtual void draw() = 0;
	virtual void update(float deltaTime);
	void regenerateLifeManaFatigue(float deltaTime);

	//init
	virtual void setCharacterType(std::string characterType);

	int getWidth() const;
	int getHeight() const;
	
	
	//behavior
	Enums::ActivityType getCurActivity() const;
	
	//interaction
	void MoveUp(unsigned short n);
	void MoveDown(unsigned short n);
	void MoveLeft(unsigned short n);
	void MoveRight(unsigned short n);
	void Move(Enums::Direction direction, unsigned short n = 1);

	void giveCoins(unsigned int amountOfCoins);
	bool CheckMouseOver(int _x_pos, int _y_pos);
	void interruptCurrentActivityWith(Enums::ActivityType activity);
	

	bool isStunned() const;
	bool isMesmerized() const;
	bool isFeared() const;
	bool isConfused() const;
	bool isCharmed() const;
	bool isChanneling() const;
	bool isSneaking() const;

	bool getIsPreparing() const;
	bool mayDoAnythingAffectingSpellActionWithoutAborting() const;
	bool mayDoAnythingAffectingSpellActionWithAborting() const;
		
	int getXPos() const;
	int getYPos() const;

	float getMovementSpeed() const;
	
	float getPreparationPercentage() const;
	std::string getCurrentSpellActionName() const;
	void addDamageDisplayToGUI(int amount, bool critical, uint8_t damageType);

	void Damage(int amount, bool criticalHit);
	bool isInvisible() const;
	void Die();
	bool isAlive() const;
	bool canBeDamaged() const;
	void Heal(int amount);

	void giveToPreparation(SpellActionBase *toPrepare);
	bool continuePreparing();
	bool castSpell(SpellActionBase *spell);
	void CastingAborted();
	void abortCurrentSpellAction();
	void startSpellAction();
	void executeSpellWithoutCasting(SpellActionBase *spell, Character *target);
	void addCooldownSpell(SpellActionBase *spell);
	void cleanupCooldownSpells();
	void addActiveSpell(SpellActionBase *spell);
	void cleanupActiveSpells();
	void removeSpellsWithCharacterState(Enums::CharacterStates characterState);
	
	const CharacterType* getCharacterType();
	std::vector<SpellActionBase*> getSpellbook() const;
	std::vector<SpellActionBase*> getActiveSpells() const;
	Enums::CharacterArchType getArchType() const;
	Character* getTarget() const;
	void setTarget(Character *target);
	

	void setBoundingBox(int bbx, int bby, int bbw, int bbh);
	void setUseBoundingBox(bool use);
	int getBoundingBoxX() const;
	int getBoundingBoxY() const;
	int getBoundingBoxW() const;
	int getBoundingBoxH() const;
	bool getUseBoundingBox() const;

	void setCurrentHealth(unsigned short newCurrentHealth);
	void setCurrentMana(unsigned short newCurrentMana);
	void setCurrentFatigue(unsigned short newCurrentFatigue);
	
	void modifyMaxHealth(short maxHealthModifier);
	void modifyMaxMana(short maxManaModifier);
	void modifyMaxFatigue(short maxFatigueModifier);
	void modifyCurrentHealth(short currentHealthModifier);
	void modifyCurrentMana(short currentManaModifier);
	void modifyCurrentFatigue(short currentFatigueModifier);

	unsigned short getModifiedManaRegen() const;
	unsigned short getModifiedHealthRegen() const;
	unsigned short getModifiedFatigueRegen() const;
	unsigned short getModifiedMaxHealth() const;
	unsigned short getModifiedMaxMana() const;
	unsigned short getModifiedMaxFatigue() const;
	unsigned short getModifiedStrength() const;
	unsigned short getModifiedDexterity() const;
	unsigned short getModifiedVitality() const;
	unsigned short getModifiedIntellect() const;
	unsigned short getModifiedWisdom() const;
	unsigned short getModifiedSpellEffectElementModifierPoints(Enums::ElementType elementType) const;
	unsigned short getModifiedResistElementModifierPoints(Enums::ElementType elementType) const;
	unsigned short getModifiedSpellCriticalModifierPoints() const;
	unsigned short getParryModifierPoints() const;
	unsigned short getBlockModifierPoints() const;
	unsigned short getMeleeCriticalModifierPoints() const;
	unsigned short getSpellCriticalModifierPoints() const;
	unsigned short getSpellEffectElementModifierPoints(Enums::ElementType elementType) const;
	unsigned short getSpellEffectAllModifierPoints() const;
	unsigned short getResistElementModifierPoints(Enums::ElementType elementType) const;
	unsigned short getResistAllModifierPoints() const;
	unsigned short getCurrentHealth() const;
	unsigned short getCurrentMana() const;
	unsigned short getCurrentFatigue() const;
	
////////////////////////////////////////////////////LUA STATES/////////////////////////////////////
	void inscribeSpellInSpellbook(SpellActionBase *spell);
	//void addItemToLootTable(Item* item, double dropChance);
	void setClass(Enums::CharacterClass characterClass);
	void setName(std::string newName);
	void setStrength(unsigned short newStrength);
	void setDexterity(unsigned short newDexterity);
	void setVitality(unsigned short newVitality);
	void setIntellect(unsigned short newIntellect);
	void setWisdom(unsigned short newWisdom);
	void setMaxHealth(unsigned short newMaxHealth);
	void setMaxMana(unsigned short newMaxMana);
	void setMaxFatigue(unsigned short newMaxFatigue);
	void setHealthRegen(unsigned short newHealthRegen);
	void setManaRegen(unsigned short newManaRegen);
	void setFatigueRegen(unsigned short newFatigueRegen);
	void setMinDamage(unsigned short newMinDamage);
	void setMaxDamage(unsigned short newMaxDamage);
	void setArmor(unsigned short newArmor);
	void setDamageModifierPoints(unsigned short newDamageModifierPoints);
	void setHitModifierPoints(unsigned short newHitModifierPoints);
	void setEvadeModifierPoints(unsigned short newEvadeModifierPoints);
	void setLevel(unsigned short newLevel);
	void setExperienceValue(unsigned short experienceValue);

	Enums::CharacterClass getClass() const;
	std::string getName() const;
	unsigned short getStrength() const;
	unsigned short getDexterity() const;
	unsigned short getVitality() const;
	unsigned short getIntellect() const;
	unsigned short getWisdom() const;
	unsigned short getMaxHealth() const;
	unsigned short getMaxMana() const;
	unsigned short getMaxFatigue() const;
	unsigned short getHealthRegen() const;
	unsigned short getManaRegen() const;
	unsigned short getFatigueRegen() const;
	unsigned short getMinDamage() const;
	unsigned short getMaxDamage() const;
	unsigned short getArmor() const;
	unsigned short getDamageModifierPoints() const;
	unsigned short getHitModifierPoints() const;
	unsigned short getEvadeModifierPoints() const;
	unsigned short getLevel() const;
	unsigned short getExperienceValue() const;

	static std::string AttitudeToString(Enums::Attitude attitude);
	static std::string ActivityToString(Enums::ActivityType activity);

protected:

	bool alive;
	bool m_isPlayer;
	bool hasChoosenFearDirection;
	bool isPreparing = false;
	// timers
	float dyingStartFrame, reduceDyingTranspFrame;

	int x_pos, y_pos;
	float preparationPercentage;
	unsigned int preparationStartTime, preparationCurrentTime;
	float progress = 0.0f;

	std::vector<SpellActionBase*> spellbook;
	std::vector<SpellActionBase*> activeSpells;
	std::vector<SpellActionBase*> cooldownSpells;
	const CharacterType* m_characterType;
	Character* target;

	SpellActionBase* curSpellAction = nullptr;
	Enums::Direction activeDirection;
	Enums::Direction lastActiveDirection;
	Enums::Direction fearDirection, dyingDirection;
	Enums::ActivityType curActivity;
	Enums::CharacterArchType characterArchType;

	const TextureRect* rect;
	unsigned short m_numActivities;
	unsigned short index = 0;
	unsigned short currentFrame = 0;
	float m_elapsedTime = 0.0f;
	float m_animationTime = 0.0f;
	bool m_waitForAnimation = false;

	unsigned short parryModifierPoints;
	unsigned short blockModifierPoints;
	unsigned short meleeCriticalModifierPoints;
	unsigned short spellCriticalModifierPoints;
	unsigned short spellEffectAllModifierPoints;
	unsigned short resistAllModifierPoints;
	unsigned short *spellEffectElementModifierPoints;
	unsigned short *resistElementModifierPoints;
	unsigned short current_health;
	unsigned short current_mana;
	unsigned short current_fatigue;
	unsigned int coins;
	float m_regenerationRate = 0.0f;

	int boundingBoxX;
	int boundingBoxY;
	int boundingBoxW;
	int boundingBoxH;
	bool useBoundingBox = false;

	//LUA STATES
	Enums::CharacterClass characterClass;
	std::string name;
	unsigned short strength;
	unsigned short dexterity;
	unsigned short vitality;
	unsigned short intellect;
	unsigned short wisdom;
	unsigned short max_health;
	unsigned short max_mana;
	unsigned short max_fatigue;
	unsigned short healthRegen;
	unsigned short manaRegen;
	unsigned short fatigueRegen;
	unsigned short min_damage;
	unsigned short max_damage;
	unsigned short armor;
	unsigned short damageModifierPoints;
	unsigned short hitModifierPoints;
	unsigned short evadeModifierPoints;
	unsigned short level;
	unsigned short experienceValue;
};