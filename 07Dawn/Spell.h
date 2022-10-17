#pragma once

#include <string>
#include <stdint.h>
#include <vector>

namespace Enums {
	enum WeaponType;
	enum CharacterStates : int;
	enum EffectType;
	enum ElementType;
	enum StatsType;
	enum CharacterClass;
}

class Character;
struct TextureRect;

class GeneralRayDamageSpell;
class GeneralAreaDamageSpell;
class GeneralBoltDamageSpell;
class GeneralHealingSpell;
class GeneralBuffSpell;
class MeleeDamageAction;
class RangedDamageAction;

class CSpellActionBase {
protected:
	CSpellActionBase();
public:
	// Question: What about different target types, such as position (for region damage spells)
	/// \brief Creates a spell to really cast as a copy from this one with a fixed target and creator.
	virtual CSpellActionBase* cast(Character *creator, Character *target, bool child = false) = 0;
	virtual CSpellActionBase* cast(Character *creator, int x, int y) = 0;
	virtual ~CSpellActionBase();

	/// Info about the spell, both for the game (mana cost, etc) and for the player (name, info, etc)

	/// \brief Returns the time needed to cast a spell.
	/// \todo Should the spell meta information be put in a separate info object?
	virtual uint16_t getCastTime() const = 0;
	virtual uint16_t getCooldown() const = 0;
	virtual uint16_t getSpellCost() const = 0;
	virtual uint16_t getDuration() const = 0;
	virtual uint16_t getRadius() const = 0;
	virtual int16_t getX() const = 0;
	virtual int16_t getY() const = 0;
	virtual bool isInRange(uint16_t distance) const = 0;
	virtual std::string getName() const = 0;
	virtual std::string getID() const;
	virtual std::string getInfo() const = 0;
	virtual TextureRect* getSymbol() const = 0;
	virtual Enums::EffectType getEffectType() const = 0;

	/// Note: The following functions may only be called on spells that where really created,
	///       i.e. for spells created by cast function.

	/// \brief Draws the graphical representation of the spell effect.
	virtual void drawEffect() = 0;

	/// \brief Called directly after creation of the spell and is thought for an initial effect.
	virtual void startEffect() = 0;
	/// \brief Called while the spell is not yet completed and thought for continuous effects.
	/// Needs to mark the spell as completed unless that has already been done,
	/// e.g. because the effect is not continuous.
	virtual void inEffect() = 0;

	/// \brief Whether the spell has done its work and the object can be destroyed.
	bool isEffectComplete() const;
	void unbindFromCreator();
	bool isBoundToCreator() const;
	void beginPreparationOfSpellAction();
	void markSpellActionAsFinished();

	/// \brief Functions for defining and playing sounds.
	void setSoundSpellCasting(std::string soundSpellCasting);
	void setSoundSpellStart(std::string soundSpellStart);
	void setSoundSpellHit(std::string soundSpellHit);
	void playSoundSpellCasting();
	void playSoundSpellStart();
	void playSoundSpellHit();
	void stopSoundSpellCasting();
	void stopSoundSpellStart();
	void stopSoundSpellHit();

	/// add additional spells to this spell, to be executed based on chance when the spell is finished.
	void addAdditionalSpellOnTarget(CSpellActionBase *spell, double chanceToExecute);
	void addAdditionalSpellOnCreator(CSpellActionBase *spell, double chanceToExecute);

	/// which class can use the spell
	void setRequiredClass(Enums::CharacterClass requiredClass);
	Enums::CharacterClass getRequiredClass() const;

	/// which equipment (weapon) is required to cast the spell.
	void addRequiredWeapon(Enums::WeaponType weaponType);
	uint32_t getRequiredWeapons() const;

	/// what level we can use the spell
	void setRequiredLevel(uint8_t requiredLevel);
	uint8_t getRequiredLevel() const;

	/// rank of the spell
	void setRank(uint8_t rank);
	uint8_t getRank() const;

	/// instant spells, doesn't require spell to "hit" the target to do damge (ranged + bolt spells)
	void setInstant(bool instant);
	bool getInstant() const;

	/// characterStates which affects the target. Stunned, charmed etc...
	void setCharacterState(Enums::CharacterStates characterState, float value = 1.0f);
	std::pair<Enums::CharacterStates, float> getCharacterState() const;

	bool isSpellHostile() const;

	void drawSymbol(int left, int bottom, int width, int height) const;

	/// this resets the luaID to empty. It is only to be used in copySpell-function because there we want to create a new spell which
	/// will be inscribed in the spell database so it will get an ID.
	void unsetLuaID();
	void setSymbolTextureRect(TextureRect& textureRect);

protected:
	Character *creator;
	Character *target;
	bool boundToCreator;
	bool finished;
	bool hostileSpell;
	bool instant; // makes the spell instant, doesn't require the spell to "hit" the target. (ranged + bolt spells)
	mutable std::string luaID; // for caching the id and not having to get it every time again
	Enums::CharacterClass requiredClass;
	std::string soundSpellCasting;
	std::string soundSpellStart;
	std::string soundSpellHit;
	uint8_t requiredLevel;
	uint32_t requiredWeapons;
	uint8_t rank;
	std::vector< std::pair<CSpellActionBase*, double> > additionalSpellsOnTarget;
	std::vector< std::pair<CSpellActionBase*, double> > additionalSpellsOnCreator;
	std::pair<Enums::CharacterStates, float> characterStateEffects;

	TextureRect* spellSymbol;
};

class CSpell : public CSpellActionBase {
public:
	CSpell() {}
};

class CAction : public CSpellActionBase {
public:
	CAction() {}
	virtual double getProgress() const = 0;
};

class GeneralBoltDamageSpell;

namespace SpellCreation {

	CSpellActionBase* getGeneralRayDamageSpell();
	CSpellActionBase* getGeneralRayDamageSpell(GeneralRayDamageSpell *other);
	CSpellActionBase* getGeneralAreaDamageSpell();
	CSpellActionBase* getGeneralAreaDamageSpell(GeneralAreaDamageSpell *other);
	CSpellActionBase* getGeneralBoltDamageSpell();
	CSpellActionBase* getGeneralBoltDamageSpell(GeneralBoltDamageSpell *other);
	CSpellActionBase* getGeneralHealingSpell();
	CSpellActionBase* getGeneralHealingSpell(GeneralHealingSpell *other);
	CSpellActionBase* getGeneralBuffSpell();
	CSpellActionBase* getGeneralBuffSpell(GeneralBuffSpell *other);
	CSpellActionBase* getMeleeDamageAction();
	CSpellActionBase* getMeleeDamageAction(MeleeDamageAction *other);
	CSpellActionBase* getRangedDamageAction();
	CSpellActionBase* getRangedDamageAction(RangedDamageAction *other);
}

class ConfigurableSpell : public CSpell {

public:
	void setCastTime(uint16_t newCastTime);
	virtual uint16_t getCastTime() const;
	void setCooldown(uint16_t newCooldown);
	virtual uint16_t getCooldown() const;
	void setSpellCost(uint16_t spellCost);
	virtual uint16_t getSpellCost() const;
	virtual uint16_t getRadius() const;
	virtual int16_t getX() const;
	virtual int16_t getY() const;
	void setRange(uint16_t minRange, uint16_t maxRange);
	virtual bool isInRange(uint16_t distance) const;
	void setName(std::string newName);
	virtual std::string getName() const;
	void setInfo(std::string newInfo);
	virtual std::string getInfo() const;
	void setDuration(uint16_t newDuration);
	virtual uint16_t getDuration() const;
	TextureRect* getSymbol() const;

protected:
	ConfigurableSpell();
	ConfigurableSpell(ConfigurableSpell *other);

	uint16_t castTime;
	uint16_t spellCost;
	uint16_t cooldown;
	uint16_t duration;
	uint16_t minRange;
	uint16_t maxRange;
	uint16_t radius;
	int16_t centerX;
	int16_t centerY;

	std::string name;
	std::string info;	
};

class ConfigurableAction : public CAction
{
public:
	void setCastTime(uint16_t newCastTime);
	virtual uint16_t getCastTime() const;
	void setCooldown(uint16_t newCooldown);
	virtual uint16_t getCooldown() const;
	void setSpellCost(uint16_t spellCost);
	virtual uint16_t getSpellCost() const;
	virtual uint16_t getRadius() const;
	virtual int16_t getX() const;
	virtual int16_t getY() const;
	void setRange(uint16_t minRange, uint16_t maxRange);
	virtual bool isInRange(uint16_t distance) const;
	void setName(std::string newName);
	virtual std::string getName() const;
	void setInfo(std::string newInfo);
	virtual std::string getInfo() const;
	void setDuration(uint16_t newDuration);
	virtual uint16_t getDuration() const;

	TextureRect* getSymbol() const;

protected:
	ConfigurableAction();
	ConfigurableAction(ConfigurableAction *other);

	uint16_t castTime;
	uint16_t spellCost;
	uint16_t cooldown;
	uint16_t duration;
	uint16_t minRange;
	uint16_t maxRange;

	std::string name;
	std::string info;
};

class GeneralDamageSpell : public ConfigurableSpell
{
public:
	void setDirectDamage(uint16_t newMinDirectDamage, uint16_t newMaxDirectDamage, Enums::ElementType newElementDirect);
	void setContinuousDamage(double newMinContDamagePerSec, double newMaxContDamagePerSec, uint16_t newContDamageTime, Enums::ElementType newContDamageElement);

	uint16_t getDirectDamageMin() const;
	uint16_t getDirectDamageMax() const;
	Enums::ElementType getDirectDamageElement() const;

	uint16_t getContinuousDamageMin() const;
	uint16_t getContinuousDamageMax() const;
	Enums::ElementType getContinuousDamageElement() const;

	virtual Enums::EffectType getEffectType() const;

	void dealDirectDamage();
	double calculateContinuousDamage(uint64_t timePassed);

protected:
	GeneralDamageSpell();
	GeneralDamageSpell(GeneralDamageSpell *other);

	uint16_t minDirectDamage; // This should be a list of effects
	uint16_t maxDirectDamage;
	Enums::ElementType elementDirect;

	double minContinuousDamagePerSecond;
	double maxContinuousDamagePerSecond;
	Enums::ElementType elementContinuous;
	uint16_t continuousDamageTime;
};

class GeneralRayDamageSpell : public GeneralDamageSpell
{
public:
	virtual CSpellActionBase* cast(Character *creator, Character *target, bool child);
	virtual CSpellActionBase* cast(Character *creator, int x, int y);

	void setNumAnimations(int count);
	void setAnimationTexture(int num, std::string filename);

	virtual void startEffect();
	virtual void inEffect();
	void finishEffect();

	virtual void drawEffect();

protected:
	GeneralRayDamageSpell();
	GeneralRayDamageSpell(GeneralRayDamageSpell *other);

private:
	friend CSpellActionBase* SpellCreation::getGeneralRayDamageSpell();
	friend CSpellActionBase* SpellCreation::getGeneralRayDamageSpell(GeneralRayDamageSpell *other);

	uint8_t frameCount;
	uint32_t effectStart;
	uint32_t lastEffect;
	uint32_t animationTimerStart;
	uint32_t animationTimerStop;
	double remainingEffect;

	int numTextures;
	std::vector<TextureRect> spellTexture;
};

class GeneralAreaDamageSpell : public GeneralDamageSpell
{
public:
	virtual CSpellActionBase* cast(Character *creator, Character *target, bool child);
	virtual CSpellActionBase* cast(Character *creator, int x, int y);

	void setNumAnimations(int count);
	void setAnimationTexture(int num, std::string filename);

	virtual void startEffect();
	virtual void inEffect();
	void finishEffect();

	virtual void drawEffect();

	Enums::EffectType getEffectType() const;

	void setRadius(uint16_t newRadius);
	virtual uint16_t getRadius();
	virtual int16_t getX();
	virtual int16_t getY();

protected:
	GeneralAreaDamageSpell();
	GeneralAreaDamageSpell(GeneralAreaDamageSpell *other);

private:
	friend CSpellActionBase* SpellCreation::getGeneralAreaDamageSpell();
	friend CSpellActionBase* SpellCreation::getGeneralAreaDamageSpell(GeneralAreaDamageSpell *other);

	uint8_t frameCount;
	uint32_t effectStart;
	uint32_t lastEffect;
	uint32_t animationTimerStart;
	uint32_t animationTimerStop;
	double remainingEffect;

	int numTextures;
	std::vector<TextureRect> spellTexture;

	Enums::EffectType effectType;
	bool child;
};

class GeneralBoltDamageSpell : public GeneralDamageSpell
{
public:
	virtual CSpellActionBase* cast(Character *creator, Character *target, bool child);
	virtual CSpellActionBase* cast(Character *creator, int x, int y);

	void setMoveSpeed(int newMoveSpeed);
	void setExpireTime(int newExpireTime);
	void setNumAnimations(int count);
	void setAnimationTexture(int num, std::string filename);

	virtual void startEffect();
	virtual void inEffect();
	void finishEffect();

	virtual void drawEffect();

protected:
	GeneralBoltDamageSpell();
	GeneralBoltDamageSpell(GeneralBoltDamageSpell *other);

private:
	friend CSpellActionBase* SpellCreation::getGeneralBoltDamageSpell();
	friend CSpellActionBase* SpellCreation::getGeneralBoltDamageSpell(GeneralBoltDamageSpell *other);

	uint32_t moveSpeed;
	uint32_t expireTime;
	int posx, posy;
	double moveRemaining;

	uint8_t frameCount;
	uint32_t effectStart;
	uint32_t lastEffect;
	uint32_t animationTimerStart;
	uint32_t animationTimerStop;

	int numBoltTextures;
	std::vector<TextureRect> boltTexture;
};

class GeneralHealingSpell : public ConfigurableSpell
{
public:
	virtual CSpellActionBase* cast(Character *creator, Character *target, bool child);
	virtual CSpellActionBase* cast(Character *creator, int x, int y);

	void setEffectType(Enums::EffectType newEffectType);
	Enums::EffectType getEffectType() const;
	void setDirectHealing(int healEffectMin, int healEffectMax, Enums::ElementType healEffectElement);
	void setContinuousHealing(double minContinuousHealingPerSecond, double maxContinuousHealingPerSecond, uint16_t continuousHealingTime, Enums::ElementType elementContinuous);
	Enums::ElementType getDirectElementType() const;
	uint16_t getDirectHealingMin() const;
	uint16_t getDirectHealingMax() const;

	Enums::ElementType getContinuousElementType() const;
	uint16_t getContinuousHealingMin() const;
	uint16_t getContinuousHealingMax() const;

	virtual void drawEffect();
	virtual void startEffect();
	virtual void inEffect();
	virtual void finishEffect();
	double calculateContinuousHealing(uint64_t timePassed);

protected:
	GeneralHealingSpell();
	GeneralHealingSpell(GeneralHealingSpell *other);

private:
	friend CSpellActionBase* SpellCreation::getGeneralHealingSpell();
	friend CSpellActionBase* SpellCreation::getGeneralHealingSpell(GeneralHealingSpell *other);

	Enums::EffectType effectType;
	uint32_t effectStart;
	uint32_t lastEffect;
	double remainingEffect;
	int healEffectMin;
	int healEffectMax;
	double minContinuousHealingPerSecond;
	double maxContinuousHealingPerSecond;
	uint16_t continuousHealingTime;

	Enums::ElementType elementContinuous;
	Enums::ElementType healEffectElement;
};

class GeneralBuffSpell : public ConfigurableSpell
{
public:
	virtual CSpellActionBase* cast(Character *creator, Character *target, bool child);
	virtual CSpellActionBase* cast(Character *creator, int x, int y);

	void setEffectType(Enums::EffectType newEffectType);
	Enums::EffectType getEffectType() const;
	int16_t getStats(Enums::StatsType statsType) const;
	void setStats(Enums::StatsType statsType, int16_t amount);
	int16_t getResistElementModifierPoints(Enums::ElementType elementType) const;
	void setResistElementModifierPoints(Enums::ElementType elementType, int16_t resistModifierPoints);
	int16_t getSpellEffectElementModifierPoints(Enums::ElementType elementType) const;
	void setSpellEffectElementModifierPoints(Enums::ElementType elementType, int16_t spellEffectElementModifierPoints);

	virtual void drawEffect();
	virtual void startEffect();
	virtual void inEffect();
	void finishEffect();

protected:
	GeneralBuffSpell();
	GeneralBuffSpell(GeneralBuffSpell *other);

private:
	friend CSpellActionBase* SpellCreation::getGeneralBuffSpell();
	friend CSpellActionBase* SpellCreation::getGeneralBuffSpell(GeneralBuffSpell *other);

	Enums::EffectType effectType;
	int16_t *statsModifier;
	int16_t *resistElementModifier;
	int16_t *spellEffectElementModifier;
	uint32_t effectStart;
};

class MeleeDamageAction : public ConfigurableAction
{
public:
	virtual CSpellActionBase* cast(Character* creator, Character* target, bool child);
	virtual CSpellActionBase* cast(Character* creator, int x, int y);

	virtual Enums::EffectType getEffectType() const;

	virtual void drawEffect();
	virtual void startEffect();
	virtual void inEffect();
	virtual void finishEffect();

	double getProgress() const;

	void setDamageBonus(double damageBonus);
	double getDamageBonus() const;
	void dealDamage();

protected:
	MeleeDamageAction();
	MeleeDamageAction(MeleeDamageAction *other);

private:
	friend CSpellActionBase* SpellCreation::getMeleeDamageAction();
	friend CSpellActionBase* SpellCreation::getMeleeDamageAction(MeleeDamageAction *other);

	uint32_t effectStart;
	double damageBonus; // How much damage bonus should we add to our min and max weapon damage?
};

class RangedDamageAction : public ConfigurableAction
{
public:
	virtual CSpellActionBase* cast(Character* creator, Character* target, bool child);
	virtual CSpellActionBase* cast(Character* creator, int x, int y);

	virtual Enums::EffectType getEffectType() const;

	virtual void drawEffect();
	virtual void startEffect();
	virtual void inEffect();
	virtual void finishEffect();

	double getProgress() const;

	void setMoveSpeed(int newMoveSpeed);
	void setExpireTime(int newExpireTime);
	void setNumAnimations(int count);
	void setAnimationTexture(int num, std::string filename);

	void setDamageBonus(double damageBonus);
	double getDamageBonus() const;
	void dealDamage();

protected:
	RangedDamageAction();
	RangedDamageAction(RangedDamageAction *other);

private:
	friend CSpellActionBase* SpellCreation::getRangedDamageAction();
	friend CSpellActionBase* SpellCreation::getRangedDamageAction(RangedDamageAction *other);

	uint32_t moveSpeed;
	uint32_t expireTime;
	int posx, posy;
	double moveRemaining;

	uint8_t frameCount;
	uint32_t effectStart;
	uint32_t lastEffect;
	uint32_t animationTimerStart;
	uint32_t animationTimerStop;

	int numProjectileTextures;
	std::vector<TextureRect> projectileTexture;

	double damageBonus; // How much damage bonus should we add to our min and max weapon damage?
};