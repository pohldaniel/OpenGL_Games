#pragma once

#include <string>
#include <vector>
#include "engine/Clock.h"
#include "engine/animation/Animation2D.h"

#include "SpellActionBase.h"

class MeleeDamageAction;
class RangedDamageAction;


namespace SpellCreation {
	CSpellActionBase* getMeleeDamageAction();
	CSpellActionBase* getMeleeDamageAction(MeleeDamageAction *other);
	CSpellActionBase* getRangedDamageAction();
	CSpellActionBase* getRangedDamageAction(RangedDamageAction *other);
}


class CAction : public CSpellActionBase {
public:
	CAction() {}
	virtual double getProgress() const = 0;
};

class ConfigurableAction : public CAction {
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
	Clock m_actionTimer;
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


class MeleeDamageAction : public ConfigurableAction {

public:
	virtual CSpellActionBase* cast(Character* creator, Character* target, bool child);
	virtual CSpellActionBase* cast(Character* creator, int x, int y);

	virtual Enums::EffectType getEffectType() const;

	virtual void draw();
	virtual void startEffect();
	virtual void inEffect(float deltatime) override;
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

class RangedDamageAction : public ConfigurableAction {

public:
	virtual CSpellActionBase* cast(Character* creator, Character* target, bool child);
	virtual CSpellActionBase* cast(Character* creator, int x, int y);

	virtual Enums::EffectType getEffectType() const;

	void draw() override;
	virtual void startEffect();
	virtual void inEffect(float deltatime) override;
	virtual void finishEffect();
	

	double getProgress() const;

	void setMoveSpeed(int newMoveSpeed);
	void setExpireTime(int newExpireTime);
	void setNumAnimations(int count);
	void setAnimationTexture(int num, std::string filename);

	void setDamageBonus(double damageBonus);
	double getDamageBonus() const;
	void dealDamage();

	void addAnimationFrame(std::string file, int paddingLeft = 0, int paddingRight = 0, int paddingTop = 0, int paddingBottom = 0);
	void update(float deltatime);
	void startAnimation();
	const bool waitForAnimation() const;

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

	Animation2D animation;
	const  TextureRect& currentFrame;
};