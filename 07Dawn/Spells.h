#pragma once

#include "SpellActionBase.h"

class GeneralRayDamageSpell;
class GeneralAreaDamageSpell;
class GeneralBoltDamageSpell;
class GeneralHealingSpell;
class GeneralBuffSpell;
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
}


class CSpell : public CSpellActionBase {
public:
	CSpell() {}
};

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

class GeneralDamageSpell : public ConfigurableSpell {
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

	void addAnimationFrame(std::string file, int paddingLeft = 0, int paddingRight = 0, int paddingTop = 0, int paddingBottom = 0);
	void update(float deltatime) override;
	void startAnimation() override;
	const bool waitForAnimation() const override;

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

	Animation2D animation;
	const  Animation2D::TextureRect& currentFrame;
};

class GeneralRayDamageSpell : public GeneralDamageSpell {

public:
	virtual CSpellActionBase* cast(Character *creator, Character *target, bool child);
	virtual CSpellActionBase* cast(Character *creator, int x, int y);

	void setNumAnimations(int count);
	void setAnimationTexture(int num, std::string filename);

	virtual void startEffect();
	virtual void inEffect();
	void finishEffect();

	virtual void drawEffect();


	void draw(int posX, int posY, float degree) override;
	float offsetRadius = 32.0f;

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

class GeneralAreaDamageSpell : public GeneralDamageSpell {
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

class GeneralBoltDamageSpell : public GeneralDamageSpell {
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

class GeneralBuffSpell : public ConfigurableSpell {

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