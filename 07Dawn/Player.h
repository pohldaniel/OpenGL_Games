#pragma once

#include "Character.h"
#include "TilesetManager.h"
#include "Inventory.h"

class Player : public Character {

public:
	Player();
	~Player();

	void draw() override;
	void update(float deltaTime) override;
	void setCharacterType(std::string characterType) override;
	void Die() override;
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

	unsigned short getModifiedMinDamage() const override;
	unsigned short getModifiedMaxDamage() const override;
	unsigned short getModifiedDamageModifierPoints() const override;
	unsigned short getModifiedSpellEffectElementModifierPoints(Enums::ElementType elementType) const override;
	unsigned short getModifiedStrength() const override;
	unsigned short getModifiedHitModifierPoints() const override;
	unsigned short getModifiedMeleeCriticalModifierPoints() const override;
	unsigned short getModifiedArmor() const override;
	unsigned short getModifiedBlockModifierPoints() const override;
	unsigned short getModifiedEvadeModifierPoints() const override;
	unsigned short getModifiedParryModifierPoints() const override;
	unsigned short getModifiedResistElementModifierPoints(Enums::ElementType elementType) const override;
	unsigned short getModifiedSpellCriticalModifierPoints() const override;
	unsigned short getModifiedDexterity() const override;
	unsigned short getModifiedVitality() const override;
	unsigned short getModifiedIntellect() const override;
	unsigned short getModifiedWisdom() const override;
	unsigned short getModifiedMaxHealth() const override;
	unsigned short getModifiedMaxMana() const override;
	unsigned short getModifiedMaxFatigue() const;
	unsigned short getModifiedHealthRegen() const override;
	unsigned short getModifiedManaRegen() const override;
	unsigned short getModifiedFatigueRegen() const override;
	unsigned short getPortraitOffset() const;
	void updatePortraitOffset();
	void setExperience(unsigned long experience);

	bool canWearArmorType(Item* item) const;
	bool isSpellInscribedInSpellbook(SpellActionBase* spell) const;
	bool isSpellOnCooldown(std::string spellName) const;

	void startShopping();
	void stopShopping();
	bool isShopping() const;
	Inventory& getInventory();
	void clearInventory();
	void clearCooldownSpells();
	void clearActiveSpells();
	void reduceCoins(unsigned int amountOfCoins);
	std::string getSaveText() const;
	static Player& Get();

	bool m_reloadItemTooltip = true;
	bool m_reloadSpellTooltip = true;

	static void DrawActiveSpells();

private:

	void Move(float deltaTime);
	void Animate(float deltaTime);
	void processInput();

	bool canRaiseLevel() const;
	void raiseLevel();

	unsigned short  movementSpeed;
	unsigned short m_portraitOffset;
	unsigned int ticketForItemTooltip;
	unsigned int ticketForSpellTooltip;

	float m_duration = 0.01f;
	float m_currentspeed = 1.0f;
	float m_inverseSpeed = 0.8f;

	unsigned long experience;

	Enums::Attitude targetAttitude;
	int dx, dy;
	bool isCurrentlyShopping;
	Inventory m_inventory;
	std::string m_characterTypeStr;

	static Player s_instance;
};