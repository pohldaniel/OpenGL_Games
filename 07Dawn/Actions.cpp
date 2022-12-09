#include "Actions.h"
#include "Enums.h"
#include "Constants.h"

#include <iostream>
#include <cstdlib>
#include <memory>
#include "Character.h"
#include "Player.h"
#include "Luafunctions.h"
#include "random.h"
#include "TextureManager.h"

#include <cassert>

namespace DawnInterface {
	//void addTextToLogWindow(GLfloat color[], const char *text, ...);
}

ConfigurableAction::ConfigurableAction() {
	spellSymbol = NULL;

	castTime = 0;
	cooldown = 0;
	spellCost = 0;
	duration = 0;
	minRange = 0;
	maxRange = 100; // default maxrange for melee actions. Can be overridden with setRange().

	name = "";
	info = "";
}

ConfigurableAction::ConfigurableAction(ConfigurableAction *other) {
	luaID = other->getID();
	spellSymbol = other->spellSymbol;

	castTime = other->castTime;
	cooldown = other->cooldown;
	spellCost = other->spellCost;
	duration = other->duration;
	minRange = other->minRange;
	maxRange = other->maxRange;
	hostileSpell = other->hostileSpell;
	instant = other->instant;

	additionalSpellsOnCreator = other->additionalSpellsOnCreator;
	additionalSpellsOnTarget = other->additionalSpellsOnTarget;

	soundSpellCasting = other->soundSpellCasting;
	soundSpellHit = other->soundSpellHit;
	soundSpellStart = other->soundSpellStart;

	characterStateEffects = other->characterStateEffects;

	requiredClass = other->requiredClass;
	requiredLevel = other->requiredLevel;
	requiredWeapons = other->requiredWeapons;

	name = other->name;
	info = other->info;
}

void ConfigurableAction::setCastTime(uint16_t newCastTime) {
	castTime = newCastTime;
}

uint16_t ConfigurableAction::getCastTime() const {
	return castTime;
}

void ConfigurableAction::setCooldown(uint16_t newCooldown) {
	cooldown = newCooldown;
}

uint16_t ConfigurableAction::getCooldown() const
{
	return cooldown;
}

uint16_t ConfigurableAction::getRadius() const
{
	return 0;
}

int16_t ConfigurableAction::getX() const
{
	return 0;
}

int16_t ConfigurableAction::getY() const
{
	return 0;
}

void ConfigurableAction::setSpellCost(uint16_t spellCost)
{
	this->spellCost = spellCost;
}

uint16_t ConfigurableAction::getSpellCost() const
{
	return spellCost;
}

void ConfigurableAction::setRange(uint16_t minRange, uint16_t maxRange)
{
	this->minRange = minRange;
	this->maxRange = maxRange;
}

bool ConfigurableAction::isInRange(double distance) const
{
	if (distance >= minRange && distance <= maxRange)
	{
		return true;
	}
	return false;
}

void ConfigurableAction::setName(std::string newName)
{
	name = newName;
}

std::string ConfigurableAction::getName() const
{
	return name;
}

void ConfigurableAction::setInfo(std::string newInfo)
{
	info = newInfo;
}

std::string ConfigurableAction::getInfo() const
{
	return info;
}

void ConfigurableAction::setDuration(uint16_t newDuration)
{
	duration = newDuration;
}

uint16_t ConfigurableAction::getDuration() const {
	return duration;
}

/// MeleeDamageAction

MeleeDamageAction::MeleeDamageAction() {
	damageBonus = 1.0;
}

MeleeDamageAction::MeleeDamageAction(MeleeDamageAction *other) : ConfigurableAction(other) {
	damageBonus = other->damageBonus;
}

SpellActionBase* MeleeDamageAction::cast(Character *creator, Character *target, bool child) {
	std::auto_ptr<MeleeDamageAction> newAction(new MeleeDamageAction(this));
	newAction->creator = creator;
	newAction->target = target;

	return newAction.release();
}

SpellActionBase* MeleeDamageAction::cast(Character *creator, int x, int y) {
	//this function does nothing... 'cause it's only needed in GeneralAreaDamageSpell
	MeleeDamageAction* newSpell = new MeleeDamageAction(this);
	return newSpell;
}

void MeleeDamageAction::setDamageBonus(double damageBonus) {
	this->damageBonus = damageBonus;
}

double MeleeDamageAction::getDamageBonus() const {
	return damageBonus;
}

void MeleeDamageAction::dealDamage() {

	/*const StatsSystem *statsSystem = StatsSystem::getStatsSystem();

	double fatigueDamageFactor = 1.0;

	// here we recalculate the damage if we're a fighter class with high fatigue
	if (creator->getArchType() == Enums::CharacterArchType::Fighter) {
	fatigueDamageFactor = 1.0 - (floor(((static_cast<double>(creator->getMaxFatigue()) - creator->getCurrentFatigue() - getSpellCost() - 1) / creator->getMaxFatigue()) / 0.25) / 10);
	if (fatigueDamageFactor > 1.0) {
	fatigueDamageFactor = 1.0;
	}
	else if (fatigueDamageFactor < 0.7) {
	fatigueDamageFactor = 0.7;
	}
	}

	double minDamage = creator->getModifiedMinDamage() * statsSystem->complexGetDamageModifier(creator->getLevel(), creator->getModifiedDamageModifierPoints(), target->getLevel());
	double maxDamage = creator->getModifiedMaxDamage() * statsSystem->complexGetDamageModifier(creator->getLevel(), creator->getModifiedDamageModifierPoints(), target->getLevel());
	int damage = RNG::randomSizeT(minDamage, maxDamage) * damageBonus;

	double hitChance = statsSystem->complexGetHitChance(creator->getLevel(), creator->getModifiedHitModifierPoints(), target->getLevel());
	double criticalHitChance = statsSystem->complexGetMeleeCriticalStrikeChance(creator->getLevel(), creator->getModifiedMeleeCriticalModifierPoints(), target->getLevel());
	double targetEvadeChance = statsSystem->complexGetEvadeChance(target->getLevel(), target->getModifiedEvadeModifierPoints(), creator->getLevel());
	double targetParryChance = statsSystem->complexGetParryChance(target->getLevel(), target->getModifiedParryModifierPoints(), creator->getLevel());
	double targetBlockChance = statsSystem->complexGetBlockChance(target->getLevel(), target->getModifiedBlockModifierPoints(), creator->getLevel());
	double damageReduction = statsSystem->complexGetDamageReductionModifier(target->getLevel(), target->getModifiedArmor(), creator->getLevel());

	bool hasHit = RNG::randomSizeT(0, 10000) <= hitChance * 10000;
	bool criticalHit = RNG::randomSizeT(0, 10000) <= criticalHitChance * 10000;
	int criticalHitFactor = 2;
	bool targetEvaded = RNG::randomSizeT(0, 10000) <= targetEvadeChance * 10000;
	bool targetParried = RNG::randomSizeT(0, 10000) <= targetParryChance * 10000;
	bool targetBlocked = RNG::randomSizeT(0, 10000) <= targetBlockChance * 10000;
	double blockFactor = 0.5;

	if (hasHit && !targetEvaded && !targetParried) {
	int damageDone = damage * (1.0 - damageReduction) * fatigueDamageFactor * (targetBlocked ? blockFactor : 1.0) * (criticalHit ? criticalHitFactor : 1);
	if (damageDone < 1) {
	damageDone = 1;
	}
	target->Damage(damageDone, criticalHit);

	/// play the hit sound effect for the spell, if we have any.
	if (soundSpellHit != "") {
	//SoundEngine::playSound(soundSpellHit);
	}
	}*/
}

double MeleeDamageAction::getProgress() const {
	return m_timer.getElapsedTimeSinceRestartMilli() / 650.0;
}

Enums::EffectType MeleeDamageAction::getEffectType() const {
	return Enums::SingleTargetSpell;
}

void MeleeDamageAction::startEffect() {

	/// play the start sound effect for the spell, if we have any.
	if (soundSpellStart != "") {
		//SoundEngine::playSound(soundSpellStart);
	}
	m_timer.restart();
	finished = false;
	target->addActiveSpell(this);
	creator->addCooldownSpell(dynamic_cast<SpellActionBase*> (cast(nullptr, nullptr, false)));
}

void MeleeDamageAction::inEffect(float deltatime) {
	if (isEffectComplete()) return;

	if (target->isAlive() == false) {
		// target died while having this effect active. mark it as finished.
		finishEffect();
		return;
	}

	if (getProgress() >= 1.0) {
		finishEffect();
	}
}

void MeleeDamageAction::draw() { }

void MeleeDamageAction::finishEffect() {
	dealDamage();
	markSpellActionAsFinished();

	// do we have an additional spell that perhaps should be cast on our target?
	for (size_t additionalSpell = 0; additionalSpell < additionalSpellsOnTarget.size(); additionalSpell++) {
		if (RNG::randomSizeT(0, 10000) <= additionalSpellsOnTarget[additionalSpell].second * 10000) {
			creator->executeSpellWithoutCasting(additionalSpellsOnTarget[additionalSpell].first, target);
		}
	}

	// do we have an additional spell that perhaps should be cast on our creator?
	for (size_t additionalSpell = 0; additionalSpell < additionalSpellsOnCreator.size(); additionalSpell++) {
		if (RNG::randomSizeT(0, 10000) <= additionalSpellsOnCreator[additionalSpell].second * 10000) {
			creator->executeSpellWithoutCasting(additionalSpellsOnCreator[additionalSpell].first, creator);
		}
	}
}

/// RangedDamageAction

RangedDamageAction::RangedDamageAction() : currentFrame(animation.getFrame()) {
	minRange = 0;
	maxRange = 360; // default maxrange for ranged attacks. Can be overridden with setRange().
	damageBonus = 1.0;
	moveSpeed = 1;
	expireTime = 10000;
}

RangedDamageAction::RangedDamageAction(RangedDamageAction *other) : ConfigurableAction(other), currentFrame(other->currentFrame) {
	minRange = other->minRange;
	maxRange = other->maxRange;
	damageBonus = other->damageBonus;
	m_animationTextures  = other->m_animationTextures;
	moveSpeed = other->moveSpeed;
	expireTime = other->expireTime;
}

SpellActionBase* RangedDamageAction::cast(Character *creator, Character *target, bool child) {
	RangedDamageAction* newSpell = new RangedDamageAction(this);
	newSpell->creator = creator;
	newSpell->target = target;

	return newSpell;
}

SpellActionBase* RangedDamageAction::cast(Character *creator, int x, int y) {
	//this function does nothing... 'cause it's only needed in GeneralAreaDamageSpell
	RangedDamageAction* newSpell = new RangedDamageAction(this);
	return newSpell;
}

void RangedDamageAction::setMoveSpeed(int newMoveSpeed) {
	moveSpeed = newMoveSpeed;
}

void RangedDamageAction::setExpireTime(int newExpireTime) {
	expireTime = newExpireTime;
}

void RangedDamageAction::startEffect() {
	/// play the start sound effect for the spell, if we have any.
	if (soundSpellStart != "") {
		//SoundEngine::playSound(soundSpellStart);
	}
	const Player& player = Player::Get();
	finished = false;

	moveRemaining = 0.0;

	posx = player.getXPos() + (player.getWidth() / 2);
	posy = player.getYPos() + (player.getHeight() / 2);

	m_timer.restart();

	target->addActiveSpell(this);
	creator->addCooldownSpell(dynamic_cast<SpellActionBase*> (cast(nullptr, nullptr, false)));
	unbindFromCreator();
}

void RangedDamageAction::inEffect(float deltatime) {
	if (isEffectComplete()) return;

	if (target->isAlive() == false) {
		// target died while having this effect active. mark it as finished.
		finishEffect();
		return;
	}

	moveRemaining += moveSpeed * deltatime;
	
	int targetx = target->getXPos() + (target->getWidth() / 2);
	int targety = target->getYPos() + (target->getHeight() / 2);
	int dx = targetx - posx;
	int dy = targety - posy;
	double dist = sqrt((dx * dx) + (dy * dy));
	double percdist = (moveRemaining / dist);
	int movex;
	int movey;

	if (percdist >= 1.0) {
		movex = dx;
		movey = dy;

	}else {
		movex = dx * percdist;
		movey = dy * percdist;
	}

	double movedDist = sqrt(movex * movex + movey * movey);
	moveRemaining -= movedDist;

	posx += movex;
	posy += movey;

	if ((posx == targetx && posy == targety) || getInstant() == true) {
		finishEffect();
	}else if (m_timer.getElapsedTimeSinceRestartMilli() > expireTime) {
		markSpellActionAsFinished();
	}
}

void RangedDamageAction::finishEffect() {
	dealDamage();
	markSpellActionAsFinished();

	// do we have an additional spell that perhaps should be cast on our target?
	for (size_t additionalSpell = 0; additionalSpell < additionalSpellsOnTarget.size(); additionalSpell++) {
		if (RNG::randomSizeT(0, 10000) <= additionalSpellsOnTarget[additionalSpell].second * 10000) {
			creator->executeSpellWithoutCasting(additionalSpellsOnTarget[additionalSpell].first, target);
		}
	}

	// do we have an additional spell that perhaps should be cast on our creator?
	for (size_t additionalSpell = 0; additionalSpell < additionalSpellsOnCreator.size(); additionalSpell++) {
		if (RNG::randomSizeT(0, 10000) <= additionalSpellsOnCreator[additionalSpell].second * 10000) {
			creator->executeSpellWithoutCasting(additionalSpellsOnCreator[additionalSpell].first, creator);
		}
	}
}

double RangedDamageAction::getProgress() const {
	return ((m_timer.getElapsedTimeSinceRestartMilli()) / 650.0);
}

void RangedDamageAction::draw() {
	if (!isEffectComplete()) {
		int targetx = target->getXPos() + (target->getWidth() / 2);
		int targety = target->getYPos() + (target->getHeight() / 2);

		float degrees = asin((posy - targety) / sqrt((pow(posx - targetx, 2) + pow(posy - targety, 2)))) * 57.296;
		degrees += 90;

		if (posx < targetx) {
			degrees = -degrees;
		}

		const TextureRect& rect = currentFrame;
		TextureManager::DrawRotatedTextureBatched(rect, static_cast<float>(posx - rect.width * 0.5f), static_cast<float>(posy - rect.height * 0.5f), degrees, rect.width * 0.5f, rect.height * 0.5f, true, 3u);

	}
}

void RangedDamageAction::setDamageBonus(double damageBonus) {
	this->damageBonus = damageBonus;
}

double RangedDamageAction::getDamageBonus() const {
	return damageBonus;
}

void RangedDamageAction::dealDamage() {

	/*const StatsSystem *statsSystem = StatsSystem::getStatsSystem();

	double fatigueDamageFactor = 1.0;

	// here we recalculate the damage if we're a fighter class with high fatigue
	if (creator->getArchType() == Enums::CharacterArchType::Fighter) {
	fatigueDamageFactor = 1.0 - (floor(((static_cast<double>(creator->getMaxFatigue()) - creator->getCurrentFatigue() - getSpellCost() - 1) / creator->getMaxFatigue()) / 0.25) / 10);
	if (fatigueDamageFactor > 1.0) {
	fatigueDamageFactor = 1.0;
	}
	else if (fatigueDamageFactor < 0.7) {
	fatigueDamageFactor = 0.7;
	}
	}

	double minDamage = creator->getModifiedMinDamage() * statsSystem->complexGetDamageModifier(creator->getLevel(), creator->getModifiedDamageModifierPoints(), target->getLevel());
	double maxDamage = creator->getModifiedMaxDamage() * statsSystem->complexGetDamageModifier(creator->getLevel(), creator->getModifiedDamageModifierPoints(), target->getLevel());
	int damage = RNG::randomSizeT(minDamage, maxDamage) * damageBonus;

	double hitChance = statsSystem->complexGetHitChance(creator->getLevel(), creator->getModifiedHitModifierPoints(), target->getLevel());
	double criticalHitChance = statsSystem->complexGetMeleeCriticalStrikeChance(creator->getLevel(), creator->getModifiedMeleeCriticalModifierPoints(), target->getLevel());
	double targetEvadeChance = statsSystem->complexGetEvadeChance(target->getLevel(), target->getModifiedEvadeModifierPoints(), creator->getLevel());
	double targetBlockChance = statsSystem->complexGetBlockChance(target->getLevel(), target->getModifiedBlockModifierPoints(), creator->getLevel());
	double damageReduction = statsSystem->complexGetDamageReductionModifier(target->getLevel(), target->getModifiedArmor(), creator->getLevel());

	bool hasHit = RNG::randomSizeT(0, 10000) <= hitChance * 10000;
	bool criticalHit = RNG::randomSizeT(0, 10000) <= criticalHitChance * 10000;
	int criticalHitFactor = 2;
	bool targetEvaded = RNG::randomSizeT(0, 10000) <= targetEvadeChance * 10000;
	bool targetBlocked = RNG::randomSizeT(0, 10000) <= targetBlockChance * 10000;
	double blockFactor = 0.5;

	if (hasHit && !targetEvaded) {
	int damageDone = damage * (1.0 - damageReduction) * fatigueDamageFactor * (targetBlocked ? blockFactor : 1.0) * (criticalHit ? criticalHitFactor : 1);
	if (damageDone < 1) {
	damageDone = 1;
	}
	target->Damage(damageDone, criticalHit);

	/// play the hit sound effect for the spell, if we have any.
	if (soundSpellHit != "") {
	//SoundEngine::playSound(soundSpellHit);
	}
	}*/
}

Enums::EffectType RangedDamageAction::getEffectType() const {
	return Enums::SingleTargetSpell;
}

void RangedDamageAction::addAnimationFrame(std::string file, int paddingLeft, int paddingRight, int paddingTop, int paddingBottom) {
	animation.addFrame(TextureManager::Loadimage(file, 0u, 0u, paddingLeft, paddingRight, paddingTop, paddingBottom));
}
void RangedDamageAction::update(float deltatime) {
	animation.update(deltatime);
}

const bool RangedDamageAction::waitForAnimation() const {
	return animation.waitForAnimation();
}

void RangedDamageAction::startAnimation() {
	animation.start();
}

namespace SpellCreation {

	SpellActionBase* getMeleeDamageAction() {
		return new MeleeDamageAction();
	}

	SpellActionBase* getMeleeDamageAction(MeleeDamageAction *other) {
		return new MeleeDamageAction(other);
	}

	SpellActionBase* getRangedDamageAction() {
		return new RangedDamageAction();
	}

	SpellActionBase* getRangedDamageAction(RangedDamageAction *other) {
		return new RangedDamageAction(other);
	}
}