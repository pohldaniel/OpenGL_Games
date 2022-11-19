#pragma once
#include "Character.h"
#include "TilesetManager.h"

class Player : public Character {

public:
	Player();
	~Player();

	void draw() override;
	void update(float deltaTime) override;
	/////////////LUA/////////////
	void init(int x, int y);

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

	unsigned int getTicketForItemTooltip() const;
	unsigned int getTicketForSpellTooltip() const;
	unsigned int getTicksOnCooldownSpell(std::string spellName) const;
	
	void setTicketForItemTooltip();
	void setTicketForSpellTooltip();

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

	static Player& Get();

private:

	void Move(float deltaTime);
	void Animate(float deltaTime);
	void processInput();

	void clearCooldownSpells();
	void clearActiveSpells();
	bool isSpellOnCooldown(std::string spellName) const;
	
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

	static Player s_instance;
};