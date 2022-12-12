#pragma once

#include "Character.h"
#include "TilesetManager.h"

class Item;
class Inventory;

class Player : public Character {

public:
	Player();
	~Player();

	void draw() override;
	void update(float deltaTime) override;
	void setCharacterType(std::string characterType) override;
	/////////////LUA/////////////
	void init();

	bool hasTarget(Character* target);

	using Character::setTarget;
	void setTarget(Character *target, Enums::Attitude attitude);

	Vector3f getPosition();
	int getDeltaX();
	int getDeltaY();

	void removeActiveSpell(SpellActionBase* activeSpell);
	std::vector<SpellActionBase*> getCooldownSpells() const;
	Enums::Attitude getTargetAttitude();

	void gainExperience(unsigned long addExp);
	unsigned long getExperience() const;
	unsigned long getExpNeededForLevel(unsigned short level) const;

	unsigned int getTicksOnCooldownSpell(std::string spellName) const;

	unsigned short getModifiedMinDamage() const;
	unsigned short getModifiedMaxDamage() const;
	unsigned short getModifiedDamageModifierPoints() const;
	unsigned short getModifiedSpellEffectElementModifierPoints(Enums::ElementType elementType) const;
	unsigned short getModifiedStrength() const;
	unsigned short getModifiedHitModifierPoints() const;
	unsigned short getModifiedMeleeCriticalModifierPoints() const;
	unsigned short getModifiedArmor() const;
	unsigned short getModifiedBlockModifierPoints() const;
	unsigned short getModifiedEvadeModifierPoints() const;
	unsigned short getModifiedParryModifierPoints() const;
	unsigned short getModifiedResistElementModifierPoints(Enums::ElementType elementType) const;
	unsigned short getModifiedSpellCriticalModifierPoints() const;
	unsigned short getModifiedDexterity() const;
	unsigned short getModifiedVitality() const;
	unsigned short getModifiedIntellect() const;
	unsigned short getModifiedWisdom() const;
	unsigned short getModifiedMaxHealth() const;
	unsigned short getModifiedMaxMana() const;
	unsigned short getModifiedMaxFatigue() const;
	unsigned short getModifiedHealthRegen() const;
	unsigned short getModifiedManaRegen() const;
	unsigned short getModifiedFatigueRegen() const;

	bool canWearArmorType(Item* item) const;
	bool isSpellInscribedInSpellbook(SpellActionBase* spell) const;
	bool isSpellOnCooldown(std::string spellName) const;

	void startShopping();
	void stopShopping();
	bool isShopping() const;
	Inventory* getInventory();
	void clearInventory();
	void reduceCoins(unsigned int amountOfCoins);

	static Player& Get();

	bool m_reloadItemTooltip = true;
	bool m_reloadSpellTooltip = true;

private:

	void Move(float deltaTime);
	void Animate(float deltaTime);
	void processInput();

	void clearCooldownSpells();
	void clearActiveSpells();

	bool canRaiseLevel() const;
	void raiseLevel();
	void setExperience(unsigned long experience);

	unsigned short  movementSpeed;
	unsigned int ticketForItemTooltip;
	unsigned int ticketForSpellTooltip;

	float m_duration = 0.01f;
	float m_currentspeed = 1.0f;
	float m_inverseSpeed = 0.8f;

	unsigned long experience;

	Enums::Attitude targetAttitude;
	int dx, dy;
	bool isCurrentlyShopping;
	Inventory* inventory;
	std::string m_characterTypeStr;

	static Player s_instance;
};