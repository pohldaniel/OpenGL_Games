#include "Spells.h"
#include "Enums.h"
#include "Statssystem.h"
#include "Constants.h"

ConfigurableSpell::ConfigurableSpell() {
	spellSymbol = NULL;

	castTime = 0;
	cooldown = 0;
	spellCost = 0;
	duration = 0;
	minRange = 0;
	maxRange = 360; // default maxrange for spells. Can be overridden with setRange().
	radius = 0;
	centerX = 0;
	centerY = 0;

	name = "";
	info = "";
}

ConfigurableSpell::ConfigurableSpell(ConfigurableSpell *other) {
	luaID = other->getID();
	spellSymbol = other->spellSymbol;

	castTime = other->castTime;
	cooldown = other->cooldown;
	spellCost = other->spellCost;
	duration = other->duration;
	minRange = other->minRange;
	maxRange = other->maxRange;
	hostileSpell = other->hostileSpell;
	radius = other->radius;
	centerX = other->centerX;
	centerY = other->centerY;
	instant = other->instant;

	additionalSpellsOnCreator = other->additionalSpellsOnCreator;
	additionalSpellsOnTarget = other->additionalSpellsOnTarget;

	characterStateEffects = other->characterStateEffects;

	soundSpellCasting = other->soundSpellCasting;
	soundSpellHit = other->soundSpellHit;
	soundSpellStart = other->soundSpellStart;

	requiredClass = other->requiredClass;
	requiredLevel = other->requiredLevel;
	requiredWeapons = other->requiredWeapons;

	name = other->name;
	info = other->info;
}

void ConfigurableSpell::setCastTime(uint16_t newCastTime) {
	castTime = newCastTime;
}

uint16_t ConfigurableSpell::getCastTime() const {
	return castTime;
}

void ConfigurableSpell::setCooldown(uint16_t newCooldown) {
	cooldown = newCooldown;
}

uint16_t ConfigurableSpell::getCooldown() const {
	return cooldown;
}

void ConfigurableSpell::setSpellCost(uint16_t spellCost)
{
	this->spellCost = spellCost;
}

uint16_t ConfigurableSpell::getSpellCost() const {
	return spellCost;
}

uint16_t ConfigurableSpell::getRadius() const {
	return radius;
}

int16_t ConfigurableSpell::getX() const {
	return centerX;
}

int16_t ConfigurableSpell::getY() const {
	return centerY;
}

void ConfigurableSpell::setRange(uint16_t minRange, uint16_t maxRange) {
	this->minRange = minRange;
	this->maxRange = maxRange;
}

bool ConfigurableSpell::isInRange(uint16_t distance) const {
	if (distance >= minRange && distance <= maxRange) {
		return true;
	}
	return maxRange < 0;
}

void ConfigurableSpell::setName(std::string newName) {
	name = newName;
}

std::string ConfigurableSpell::getName() const {
	return name;
}

void ConfigurableSpell::setInfo(std::string newInfo) {
	info = newInfo;
}

std::string ConfigurableSpell::getInfo() const {
	return info;
}

void ConfigurableSpell::setDuration(uint16_t newDuration) {
	duration = newDuration;
}

uint16_t ConfigurableSpell::getDuration() const {
	return duration;
}

TextureRect* ConfigurableSpell::getSymbol() const {
	return spellSymbol;
}

/// GeneralDamageSpell
GeneralDamageSpell::GeneralDamageSpell() : currentFrame(animation.getFrame()) {
	minDirectDamage = 0;
	maxDirectDamage = 0;
	elementDirect = Enums::ElementType::Air;

	minContinuousDamagePerSecond = 0.0;
	maxContinuousDamagePerSecond = 0.0;
	elementContinuous = Enums::ElementType::Air;
}

GeneralDamageSpell::GeneralDamageSpell(GeneralDamageSpell *other) : ConfigurableSpell(other), currentFrame(animation.getFrame()) {
	minDirectDamage = other->minDirectDamage; // This should be a list of effects
	maxDirectDamage = other->maxDirectDamage;
	elementDirect = other->elementDirect;

	minContinuousDamagePerSecond = other->minContinuousDamagePerSecond;
	maxContinuousDamagePerSecond = other->maxContinuousDamagePerSecond;
	elementContinuous = other->elementContinuous;
	continuousDamageTime = other->continuousDamageTime;

	animation = other->animation;
}

void GeneralDamageSpell::setDirectDamage(uint16_t newMinDirectDamage, uint16_t newMaxDirectDamage, Enums::ElementType newElementDirect) {
	minDirectDamage = newMinDirectDamage; // This should be a list of effects
	maxDirectDamage = newMaxDirectDamage;
	elementDirect = newElementDirect;
}

void GeneralDamageSpell::setContinuousDamage(double newMinContDamagePerSec, double newMaxContDamagePerSec, uint16_t newContDamageTime, Enums::ElementType newContDamageElement) {
	minContinuousDamagePerSecond = newMinContDamagePerSec;
	maxContinuousDamagePerSecond = newMaxContDamagePerSec;
	elementContinuous = newContDamageElement;
	continuousDamageTime = newContDamageTime;

	// setting a continous damage also sets a duration of the spell (so we can see it in the buffwindow)
	setDuration(static_cast<uint16_t> (floor(continuousDamageTime / 1000)));
}

Enums::EffectType GeneralDamageSpell::getEffectType() const {
	return Enums::EffectType::SingleTargetSpell;
}

uint16_t GeneralDamageSpell::getDirectDamageMin() const {
	return minDirectDamage;
}

uint16_t GeneralDamageSpell::getDirectDamageMax() const {
	return maxDirectDamage;
}

Enums::ElementType GeneralDamageSpell::getDirectDamageElement() const {
	return elementDirect;
}

uint16_t GeneralDamageSpell::getContinuousDamageMin() const {
	return minContinuousDamagePerSecond;
}

uint16_t GeneralDamageSpell::getContinuousDamageMax() const {
	return maxContinuousDamagePerSecond;
}

Enums::ElementType GeneralDamageSpell::getContinuousDamageElement() const {
	return elementContinuous;
}

void GeneralDamageSpell::dealDirectDamage() {
	if (getDirectDamageMax() > 0) {
	
		/// play the hit sound effect for the spell, if we have any.
		if (soundSpellHit != "") {
		//SoundEngine::playSound(soundSpellHit);
		}

		int damage = getDirectDamageMin() + RNG::randomInt(0, getDirectDamageMax() - getDirectDamageMin());
		double fatigueDamageFactor = 1.0;

		// here we recalculate the damage if we're a fighter class with high fatigue
		if (creator->getArchType() == Enums::CharacterArchType::Fighter) {
			fatigueDamageFactor = 1.0 - (floor(((static_cast<double>(creator->getMaxFatigue()) - creator->getCurrentFatigue() - getSpellCost() - 1) / creator->getMaxFatigue()) / 0.25) / 10);
			if (fatigueDamageFactor > 1.0) {
				fatigueDamageFactor = 1.0;
			}else if (fatigueDamageFactor < 0.7) {
				fatigueDamageFactor = 0.7;
			}
		}

		double damageFactor = StatsSystem::getStatsSystem()->complexGetSpellEffectElementModifier(creator->getLevel(), creator->getModifiedSpellEffectElementModifierPoints(getDirectDamageElement()), target->getLevel());
		double resist = StatsSystem::getStatsSystem()->complexGetResistElementChance(target->getLevel(), target->getModifiedResistElementModifierPoints(getDirectDamageElement()), creator->getLevel());
		double realDamage = damage * damageFactor * fatigueDamageFactor * (1 - resist);
		double spellCriticalChance = StatsSystem::getStatsSystem()->complexGetSpellCriticalStrikeChance(creator->getLevel(), creator->getModifiedSpellCriticalModifierPoints(), target->getLevel());
		bool criticalHit = RNG::randomSizeT(0, 10000) <= spellCriticalChance * 10000;
		
		if (criticalHit == true) {
			int criticalDamageMultiplier = 2;
			realDamage *= criticalDamageMultiplier;
		}

		if (target->isAlive()) {
			target->Damage(round(realDamage), criticalHit);
		}
	}
}

double GeneralDamageSpell::calculateContinuousDamage(uint64_t timePassed) {
	double secondsPassed = (timePassed) / 1000.0;

	double curRandDamage = RNG::randomDouble(minContinuousDamagePerSecond * secondsPassed, maxContinuousDamagePerSecond * secondsPassed);

	double damageFactor = StatsSystem::getStatsSystem()->complexGetSpellEffectElementModifier(creator->getLevel(), creator->getModifiedSpellEffectElementModifierPoints(elementContinuous), target->getLevel());
	double resist = StatsSystem::getStatsSystem()->complexGetResistElementChance(target->getLevel(), target->getModifiedResistElementModifierPoints(elementContinuous), creator->getLevel());
	double realDamage = curRandDamage * damageFactor * (1 - resist);
	return realDamage;
	return 1.0;
}

void GeneralDamageSpell::addAnimationFrame(std::string file, int paddingLeft, int paddingRight, int paddingTop, int paddingBottom) {
	animation.addFrame(TextureManager::Loadimage(file, 0u, 0u, false, paddingLeft, paddingRight, paddingTop, paddingBottom));
}
void GeneralDamageSpell::update(float deltatime) {
	animation.update(deltatime);
}

const bool GeneralDamageSpell::waitForAnimation() const {
	return animation.waitForAnimation();
}

void GeneralDamageSpell::startAnimation() {
	animation.start();
}

/// class GeneralRayDamageSpell
GeneralRayDamageSpell::GeneralRayDamageSpell() {
	remainingEffect = 0;
	numTextures = 0;
}

GeneralRayDamageSpell::GeneralRayDamageSpell(GeneralRayDamageSpell *other) : GeneralDamageSpell(other){
	remainingEffect = 0;

	numTextures = other->numTextures;
	spellTexture = other->spellTexture;	
}

CSpellActionBase* GeneralRayDamageSpell::cast(Character *creator, Character *target, bool child) {

	GeneralRayDamageSpell* newSpell = new GeneralRayDamageSpell(this);
	newSpell->creator = creator;
	newSpell->target = target;

	return newSpell;
}

CSpellActionBase* GeneralRayDamageSpell::cast(Character *creator, int x, int y) {

	//this function does nothing... 'cause it's only needed in GeneralAreaDamageSpell
	GeneralRayDamageSpell* newSpell = new GeneralRayDamageSpell(this);
	return newSpell;
}

void GeneralRayDamageSpell::setNumAnimations(int count) {
	numTextures = count;
}

void GeneralRayDamageSpell::setAnimationTexture(int num, std::string filename) {
	TextureManager::Loadimage(filename, num, spellTexture);
}

void GeneralRayDamageSpell::startEffect() {
	if (!isEffectComplete()) return;
	/// play the start sound effect for the spell, if we have any.
	if (soundSpellStart != "") {
		//SoundEngine::playSound(soundSpellStart);
	}
	animation.start();
	finished = false;

	remainingEffect = 0.0;
	frameCount = 0;

	dealDirectDamage();

	effectStart = Globals::clock.getElapsedTimeMilli();
	animationTimerStart = effectStart;
	lastEffect = effectStart;

	m_spellTimer.restart();

	
	target->addActiveSpell(this);
	creator->addCooldownSpell(dynamic_cast<CSpellActionBase*> (cast(nullptr, nullptr, false)));
	unbindFromCreator();
}

void GeneralRayDamageSpell::inEffect(float deltatime) {
	
	if (isEffectComplete()) return;

	if (target->isAlive() == false) {
		// target died while having this effect active. mark it as finished.
		finishEffect();
		return;
	}


	animation.update(deltatime);

	unsigned int elapsedSinceLast = m_spellTimer.getElapsedTimeMilli();
	if (elapsedSinceLast < 1000) {
		return;
	}
	m_spellTimer.reset();

	remainingEffect += calculateContinuousDamage(elapsedSinceLast);

	// no critical damage in this phase so far
	bool callFinish = false;
	if (m_spellTimer.getElapsedTimeSinceRestartMilli() >= continuousDamageTime) {
		callFinish = true;
	}

	if (floor(remainingEffect) > 0) {
		//target->Damage(floor(remainingEffect), false);
		remainingEffect = remainingEffect - floor(remainingEffect);
	}

	if (callFinish ) {
		finishEffect();
	}	
}

void GeneralRayDamageSpell::finishEffect() {

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

void GeneralRayDamageSpell::draw() {
	if (!isEffectComplete()) {

		degrees = asin((Player::Get().getYPos() - target->getYPos()) / sqrt((pow(Player::Get().getXPos() - target->getXPos(), 2) + pow(Player::Get().getYPos() - target->getYPos(), 2)))) * 57.296;
		degrees += 90;

		if (Player::Get().getXPos() < target->getXPos()) {
			degrees = -degrees;
		}

		TextureManager::BindTexture(TextureManager::GetTextureAtlas("spells"), true);
		const TextureRect& rect = currentFrame;	
		TextureManager::DrawRotatedTexture(rect, static_cast<float>(Player::Get().getXPos() - 128), static_cast<float>(Player::Get().getYPos() + 32), degrees, rect.width * 0.5f + offsetRadius, -offsetRadius);
		TextureManager::UnbindTexture(true);
	}
}

/// class GeneralAreaDamageSpell
int16_t GeneralAreaDamageSpell::getX() {
	return centerX;
}

int16_t GeneralAreaDamageSpell::getY() {
	return centerY;
}

uint16_t GeneralAreaDamageSpell::getRadius() {
	return radius;
}

GeneralAreaDamageSpell::GeneralAreaDamageSpell() {
	remainingEffect = 0;

	numTextures = 0;

	centerX = 0;
	centerY = 0;
	radius = 0;
	effectType = Enums::EffectType::AreaTargetSpell;
	child = false;
}

GeneralAreaDamageSpell::GeneralAreaDamageSpell(GeneralAreaDamageSpell *other) : GeneralDamageSpell(other) {
	remainingEffect = 0;

	numTextures = other->numTextures;
	spellTexture = other->spellTexture;
	spellSymbol = other->spellSymbol;

	castTime = other->castTime;
	cooldown = other->cooldown;
	spellCost = other->spellCost;
	duration = other->duration;
	minRange = other->minRange;
	maxRange = other->maxRange;
	hostileSpell = other->hostileSpell;
	radius = other->radius;
	centerX = other->centerX;
	centerY = other->centerY;
	instant = other->instant;

	additionalSpellsOnCreator = other->additionalSpellsOnCreator;
	additionalSpellsOnTarget = other->additionalSpellsOnTarget;

	characterStateEffects = other->characterStateEffects;

	soundSpellCasting = other->soundSpellCasting;
	soundSpellHit = other->soundSpellHit;
	soundSpellStart = other->soundSpellStart;

	requiredClass = other->requiredClass;
	requiredLevel = other->requiredLevel;
	requiredWeapons = other->requiredWeapons;

	name = other->name;
	info = other->info;
}

CSpellActionBase* GeneralAreaDamageSpell::cast(Character *creator, Character *target, bool child) {

	GeneralAreaDamageSpell* newSpell = new GeneralAreaDamageSpell(this);
	newSpell->creator = creator;
	newSpell->centerX = target->getXPos() + target->getWidth() / 2;
	newSpell->centerY = target->getYPos() + target->getHeight() / 2;
	newSpell->child = child;
	newSpell->target = target;

	return newSpell;
}

CSpellActionBase* GeneralAreaDamageSpell::cast(Character *creator, int x, int y) {

	GeneralAreaDamageSpell* newSpell = new GeneralAreaDamageSpell(this);

	newSpell->creator = creator;
	newSpell->centerX = x;
	newSpell->centerY = y;
	newSpell->child = false;

	return newSpell;
}

void GeneralAreaDamageSpell::setNumAnimations(int count) {
	numTextures = count;
}

void GeneralAreaDamageSpell::setAnimationTexture(int num, std::string filename) {
	TextureManager::Loadimage(filename, num, spellTexture);
}

void GeneralAreaDamageSpell::startEffect() {
	if (!isEffectComplete()) return;
	/// play the start sound effect for the spell, if we have any.
	if (soundSpellStart != "") {
		//SoundEngine::playSound(soundSpellStart);
	}

	radius = 50;
	remainingEffect = 0.0;
	frameCount = 0;
	finished = false;
	m_spellTimer.restart();
	
	m_elapsedTime = 0.0f;
	if (child) dealDirectDamage();

	effectStart = Globals::clock.getElapsedTimeMilli();
	animationTimerStart = Globals::clock.getElapsedTimeMilli();
	lastEffect = Globals::clock.getElapsedTimeMilli();

	if (!child) {
		ZoneManager::Get().getCurrentZone()->addActiveAoESpell(this);

		if (creator->getTarget() != NULL)
			creator->addCooldownSpell(dynamic_cast<CSpellActionBase*> (cast(NULL, creator->getTarget(), false)));
		else
			creator->addCooldownSpell(dynamic_cast<CSpellActionBase*> (cast(NULL, centerX, centerY)));

		unbindFromCreator();
	}
}

void GeneralAreaDamageSpell::inEffect(float deltaTime) {
	if (isEffectComplete()) return;
	
	m_elapsedTime = m_elapsedTime + deltaTime * 10;
	radius = static_cast<unsigned short>(floor(m_elapsedTime)) + 50;

	unsigned int elapsedSinceLast = m_spellTimer.getElapsedTimeMilli();
	if (elapsedSinceLast < 1000) {		
		return;
	}
	m_spellTimer.reset();

	if (child) remainingEffect += calculateContinuousDamage(elapsedSinceLast);
	// no critical damage in this phase so far

	bool callFinish = false;
	if (m_spellTimer.getElapsedTimeSinceRestartMilli() >= continuousDamageTime) {
		callFinish = true;
	}

	if (floor(remainingEffect) > 0 && child) {
		
		target->Damage(floor(remainingEffect), false);
		remainingEffect = remainingEffect - floor(remainingEffect);
		if (!target->isAlive()) {
			callFinish = true;
		}
	}

	if (callFinish) {
		finishEffect();
	}
}

void GeneralAreaDamageSpell::finishEffect() {
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

void GeneralAreaDamageSpell::draw() {
	if (!isEffectComplete() && !child) {
		TextureManager::BindTexture(TextureManager::GetTextureAtlas("spells"), true);
		const TextureRect& rect = currentFrame;
		
		TextureManager::DrawTexture(rect, centerX - radius, centerY - radius, radius * 2, radius * 2, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false, true);
		TextureManager::UnbindTexture(true);
	}
}

Enums::EffectType GeneralAreaDamageSpell::getEffectType() const {
	return effectType;
}

void GeneralAreaDamageSpell::setRadius(uint16_t newRadius) {
	radius = newRadius;
}

/// class GeneralBoltDamageSpell
GeneralBoltDamageSpell::GeneralBoltDamageSpell() {
	numBoltTextures = 0;
	moveSpeed = 1;
	expireTime = 10000;
}

GeneralBoltDamageSpell::GeneralBoltDamageSpell(GeneralBoltDamageSpell *other) : GeneralDamageSpell(other) {
	numBoltTextures = other->numBoltTextures;
	boltTexture = other->boltTexture;
	moveSpeed = other->moveSpeed;
	expireTime = other->expireTime;
}

CSpellActionBase* GeneralBoltDamageSpell::cast(Character *creator, Character *target, bool child) {
	GeneralBoltDamageSpell* newSpell = new GeneralBoltDamageSpell(this);
	newSpell->creator = creator;
	newSpell->target = target;

	return newSpell;
}

CSpellActionBase* GeneralBoltDamageSpell::cast(Character *creator, int x, int y) {
	//this function does nothing... 'cause it's only needed in GeneralAreaDamageSpell
	GeneralBoltDamageSpell* newSpell = new GeneralBoltDamageSpell(this);
	return newSpell;
}

void GeneralBoltDamageSpell::setMoveSpeed(int newMoveSpeed) {
	moveSpeed = newMoveSpeed;
}

void GeneralBoltDamageSpell::setExpireTime(int newExpireTime) {
	expireTime = newExpireTime;
}

void GeneralBoltDamageSpell::setNumAnimations(int count) {
	numBoltTextures = count;
}

void GeneralBoltDamageSpell::setAnimationTexture(int num, std::string filename) {
	TextureManager::Loadimage(filename, num, boltTexture);
}

void GeneralBoltDamageSpell::startEffect() {
	/// play the start sound effect for the spell, if we have any.
	if (soundSpellStart != "") {
		//SoundEngine::playSound(soundSpellStart);
	}

	if(animation.getNumberOfFrames() > 1u)
		animation.start();

	const Player& player = Player::Get();
	finished = false;

	frameCount = 0;
	moveRemaining = 0.0;
	effectStart = Globals::clock.getElapsedTimeMilli();
	animationTimerStart = effectStart;
	lastEffect = effectStart;
	posx = player.getXPos() + (player.getWidth() / 2);
	posy = player.getYPos() + (player.getHeight() / 2);

	m_spellTimer.restart();
	
	target->addActiveSpell(this);
	creator->addCooldownSpell(dynamic_cast<CSpellActionBase*> (cast(nullptr, nullptr, false)));
	unbindFromCreator();
}

void GeneralBoltDamageSpell::inEffect(float deltatime) {
	
	if (isEffectComplete()) return;

	if (animation.getNumberOfFrames() > 1u)
		animation.update(deltatime);

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
	}else if (m_spellTimer.getElapsedTimeSinceRestartMilli() > expireTime) {
		markSpellActionAsFinished();
	}
}

void GeneralBoltDamageSpell::finishEffect() {
	dealDirectDamage();
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

void GeneralBoltDamageSpell::draw() {
	if (!isEffectComplete()) {
		int targetx = target->getXPos() + (target->getWidth() / 2);
		int targety = target->getYPos() + (target->getHeight() / 2);

		float degrees = asin((posy - targety) / sqrt((pow(posx - targetx, 2) + pow(posy - targety, 2)))) * 57.296;
		degrees += 90;

		if (posx < targetx) {
			degrees = -degrees;
		}

		TextureManager::BindTexture(TextureManager::GetTextureAtlas("spells"), true);
		const TextureRect& rect = currentFrame;
		TextureManager::DrawRotatedTexture(rect, static_cast<float>(posx - rect.width * 0.5f), static_cast<float>(posy - rect.height * 0.5f), degrees, rect.width * 0.5f, rect.height * 0.5f);
		TextureManager::UnbindTexture(true);		
	}
}

/// GeneralHealingSpell
GeneralHealingSpell::GeneralHealingSpell() {
	effectType = Enums::SelfAffectingSpell;
	healEffectMin = 0;
	healEffectMax = 0;
	healEffectElement = Enums::ElementType::Light;
	minContinuousHealingPerSecond = 0.0;
	maxContinuousHealingPerSecond = 0.0;
	elementContinuous = Enums::ElementType::Light;
	remainingEffect = 0.0;
	lastEffect = effectStart;
	continuousHealingTime = 0;
	hostileSpell = false;
}

GeneralHealingSpell::GeneralHealingSpell(GeneralHealingSpell *other) : ConfigurableSpell(other) {
	effectType = other->effectType;
	healEffectMin = other->healEffectMin;
	healEffectMax = other->healEffectMax;
	healEffectElement = other->healEffectElement;
	minContinuousHealingPerSecond = other->minContinuousHealingPerSecond;
	maxContinuousHealingPerSecond = other->maxContinuousHealingPerSecond;
	elementContinuous = other->elementContinuous;
	remainingEffect = other->remainingEffect;
	continuousHealingTime = other->continuousHealingTime;
}

CSpellActionBase* GeneralHealingSpell::cast(Character *creator, Character *target, bool child) {

	std::auto_ptr<GeneralHealingSpell> newSpell(new GeneralHealingSpell(this));
	newSpell->creator = creator;
	newSpell->target = target;

	return newSpell.release();
}

CSpellActionBase* GeneralHealingSpell::cast(Character *creator, int x, int y) {
	//this function does nothing... 'cause it's only needed in GeneralAreaDamageSpell
	GeneralHealingSpell* newSpell = new GeneralHealingSpell(this);
	return newSpell;
}

void GeneralHealingSpell::setEffectType(Enums::EffectType newEffectType) {
	effectType = newEffectType;
}

Enums::EffectType GeneralHealingSpell::getEffectType() const {
	return effectType;
}

Enums::ElementType GeneralHealingSpell::getDirectElementType() const {
	return healEffectElement;
}

uint16_t GeneralHealingSpell::getDirectHealingMin() const {
	return healEffectMin;
}

uint16_t GeneralHealingSpell::getDirectHealingMax() const {
	return healEffectMax;
}

Enums::ElementType GeneralHealingSpell::getContinuousElementType() const {
	return elementContinuous;
}

uint16_t GeneralHealingSpell::getContinuousHealingMin() const {
	return minContinuousHealingPerSecond;
}

uint16_t GeneralHealingSpell::getContinuousHealingMax() const {
	return maxContinuousHealingPerSecond;
}

void GeneralHealingSpell::setDirectHealing(int healEffectMin, int healEffectMax, Enums::ElementType healEffectElement) {
	this->healEffectMin = healEffectMin;
	this->healEffectMax = healEffectMax;
	this->healEffectElement = healEffectElement;
}

void GeneralHealingSpell::setContinuousHealing(double minContinuousHealingPerSecond, double maxContinuousHealingPerSecond, uint16_t continuousHealingTime, Enums::ElementType elementContinuous) {
	this->minContinuousHealingPerSecond = minContinuousHealingPerSecond;
	this->maxContinuousHealingPerSecond = maxContinuousHealingPerSecond;
	this->continuousHealingTime = continuousHealingTime;
	this->elementContinuous = elementContinuous;

	// setting a continous heal also sets a duration of the spell (so we can see it in the buffwindow)
	setDuration(static_cast<uint16_t> (floor(continuousHealingTime / 1000)));
}

double GeneralHealingSpell::calculateContinuousHealing(uint64_t timePassed) {
	double secondsPassed = (timePassed) / 1000.0;

	double curRandHealing = RNG::randomDouble(minContinuousHealingPerSecond * secondsPassed, maxContinuousHealingPerSecond * secondsPassed);

	double healingFactor = StatsSystem::getStatsSystem()->complexGetSpellEffectElementModifier(creator->getLevel(), creator->getModifiedSpellEffectElementModifierPoints(elementContinuous), target->getLevel());
	double realHealing = curRandHealing;
	return realHealing;
}

void GeneralHealingSpell::startEffect() {
	/// play the start sound effect for the spell, if we have any.
	if (soundSpellStart != "") {
		//SoundEngine::playSound(soundSpellStart);
	}

	finished = false;

	remainingEffect = 0.0;
	effectStart = Globals::clock.getElapsedTimeMilli();
	lastEffect = effectStart;

	m_spellTimer.restart();

	int healing = RNG::randomSizeT(healEffectMin, healEffectMax);

	// only do a heal if we've set a healing value to the spell.
	if ((healEffectMin + healEffectMax) > 0) {
		double healingFactor = StatsSystem::getStatsSystem()->complexGetSpellEffectElementModifier(creator->getLevel(), creator->getModifiedSpellEffectElementModifierPoints(healEffectElement), creator->getLevel());
		double realHealing = healing * healingFactor;
		double spellCriticalChance = StatsSystem::getStatsSystem()->complexGetSpellCriticalStrikeChance(creator->getLevel(), creator->getModifiedSpellCriticalModifierPoints(), creator->getLevel());
		if (RNG::randomSizeT(0, 10000) <= spellCriticalChance * 10000) {
		int criticalEffectMultiplier = 2;
			realHealing *= criticalEffectMultiplier;
		}
		int healingCaused = round(realHealing);

		target->Heal(healingCaused);
	}

	target->addActiveSpell(this);
	creator->addCooldownSpell(dynamic_cast<CSpellActionBase*> (cast(nullptr, nullptr, false)));
	unbindFromCreator();
}

void GeneralHealingSpell::inEffect(float deltatime) {
	if (isEffectComplete()) return;

	unsigned int elapsedSinceLast = m_spellTimer.getElapsedTimeMilli();
	if (elapsedSinceLast < 1000) {
		return;
	}
	m_spellTimer.reset();

	remainingEffect += calculateContinuousHealing(elapsedSinceLast);

	bool callFinish = false;
	if (m_spellTimer.getElapsedTimeSinceRestartMilli() >= continuousHealingTime) {
		callFinish = true;
	}

	if (floor(remainingEffect) > 0) {
		target->Heal(floor(remainingEffect));
		remainingEffect = remainingEffect - floor(remainingEffect);
	}
	
	if (callFinish) {
		finishEffect();
	}
}

void GeneralHealingSpell::finishEffect() {
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

void GeneralHealingSpell::draw() { }

/// GeneralBuffSpell
GeneralBuffSpell::GeneralBuffSpell() {
	effectType = Enums::EffectType::SelfAffectingSpell;

	hostileSpell = false;

	resistElementModifier = new int16_t[static_cast<size_t>(Enums::ElementType::CountET)];
	spellEffectElementModifier = new int16_t[static_cast<size_t>(Enums::ElementType::CountET)];
	statsModifier = new int16_t[static_cast<size_t>(Enums::StatsType::CountST)];

	for (size_t curElement = 0; curElement<static_cast<size_t>(Enums::ElementType::CountET); ++curElement) {
		resistElementModifier[curElement] = 0;
		spellEffectElementModifier[curElement] = 0;
	}

	for (size_t curStat = 0; curStat<static_cast<size_t>(Enums::StatsType::CountST); ++curStat) {
		statsModifier[curStat] = 0;
	}
}

GeneralBuffSpell::GeneralBuffSpell(GeneralBuffSpell *other) : ConfigurableSpell(other) {

	effectType = other->effectType;
	resistElementModifier = new int16_t[static_cast<size_t>(Enums::ElementType::CountET)];
	spellEffectElementModifier = new int16_t[static_cast<size_t>(Enums::ElementType::CountET)];
	statsModifier = new int16_t[static_cast<size_t>(Enums::StatsType::CountST)];

	for (size_t curElement = 0; curElement<static_cast<size_t>(Enums::ElementType::CountET); ++curElement) {
		resistElementModifier[curElement] = other->resistElementModifier[curElement];
		spellEffectElementModifier[curElement] = other->spellEffectElementModifier[curElement];
	}

	for (size_t curStat = 0; curStat<static_cast<size_t>(Enums::StatsType::CountST); ++curStat) {
		statsModifier[curStat] = other->statsModifier[curStat];
	}
}

CSpellActionBase* GeneralBuffSpell::cast(Character *creator, Character *target, bool child) {
	std::auto_ptr<GeneralBuffSpell> newSpell(new GeneralBuffSpell(this));
	newSpell->creator = creator;
	newSpell->target = target;

	return newSpell.release();
}

CSpellActionBase* GeneralBuffSpell::cast(Character *creator, int x, int y) {
	//this function does nothing... 'cause it's only needed in GeneralAreaDamageSpell
	GeneralBuffSpell* newSpell = new GeneralBuffSpell(this);
	return newSpell;
}

void GeneralBuffSpell::setEffectType(Enums::EffectType newEffectType) {
	effectType = newEffectType;
}

Enums::EffectType GeneralBuffSpell::getEffectType() const {
	return effectType;
}

int16_t GeneralBuffSpell::getStats(Enums::StatsType statsType) const {
	return statsModifier[static_cast<size_t>(statsType)];
}

int16_t GeneralBuffSpell::getResistElementModifierPoints(Enums::ElementType elementType) const {
	return resistElementModifier[static_cast<size_t>(elementType)];
}

int16_t GeneralBuffSpell::getSpellEffectElementModifierPoints(Enums::ElementType elementType) const {
	return spellEffectElementModifier[static_cast<size_t>(elementType)];
}

void GeneralBuffSpell::setStats(Enums::StatsType statsType, int16_t amount) {
	// here we set the state of the spell. If the amount is negative, the spell is hostile.
	if (amount < 0) {
		hostileSpell = true;
	}
	statsModifier[static_cast<size_t>(statsType)] = amount;
}

void GeneralBuffSpell::setResistElementModifierPoints(Enums::ElementType elementType, int16_t resistModifierPoints) {
	// here we set the state of the spell. If the amount is negative, the spell is hostile.
	if (resistModifierPoints < 0) {
		hostileSpell = true;
	}
	resistElementModifier[static_cast<size_t>(elementType)] = resistModifierPoints;
}

void GeneralBuffSpell::setSpellEffectElementModifierPoints(Enums::ElementType elementType, int16_t spellEffectElementModifierPoints) {
	// here we set the state of the spell. If the amount is negative, the spell is hostile.
	if (spellEffectElementModifierPoints < 0) {
		hostileSpell = true;
	}
	spellEffectElementModifier[static_cast<size_t>(elementType)] = spellEffectElementModifierPoints;
}

void GeneralBuffSpell::startEffect() {
	/// play the start sound effect for the spell, if we have any.
	if (soundSpellStart != "") {
		//SoundEngine::playSound(soundSpellStart);
	}
	m_spellTimer.restart();
	finished = false;

	effectStart = Globals::clock.getElapsedTimeMilli();
	target->addActiveSpell(this);
	creator->addCooldownSpell(dynamic_cast<CSpellActionBase*> (cast(nullptr, nullptr, false)));
	unbindFromCreator();
}

void GeneralBuffSpell::inEffect(float deltatime) {
	
	if (isEffectComplete()) return;

	if (target->isAlive() == false) {
		// target died while having this effect active. mark it as finished.
		finishEffect();
		return;
	}
	
	uint32_t curTime = Globals::clock.getElapsedTimeMilli();
	if (m_spellTimer.getElapsedTimeSinceRestartMilli() > getDuration() * 1000u) {
		finishEffect();
	}
}

void GeneralBuffSpell::draw() { }

void GeneralBuffSpell::finishEffect() {
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

namespace SpellCreation {

	CSpellActionBase* getGeneralRayDamageSpell() {
		return new GeneralRayDamageSpell();
	}

	CSpellActionBase* getGeneralRayDamageSpell(GeneralRayDamageSpell *other) {
		return new GeneralRayDamageSpell(other);
	}

	CSpellActionBase* getGeneralAreaDamageSpell() {
		return new GeneralAreaDamageSpell();
	}

	CSpellActionBase* getGeneralAreaDamageSpell(GeneralAreaDamageSpell *other) {
		return new GeneralAreaDamageSpell(other);
	}

	CSpellActionBase* getGeneralBoltDamageSpell() {
		return new GeneralBoltDamageSpell();
	}

	CSpellActionBase* getGeneralBoltDamageSpell(GeneralBoltDamageSpell *other) {
		return new GeneralBoltDamageSpell(other);
	}

	CSpellActionBase* getGeneralHealingSpell() {
		return new GeneralHealingSpell();
	}

	CSpellActionBase* getGeneralHealingSpell(GeneralHealingSpell *other) {
		return new GeneralHealingSpell(other);
	}

	CSpellActionBase* getGeneralBuffSpell() {
		return new GeneralBuffSpell();
	}

	CSpellActionBase* getGeneralBuffSpell(GeneralBuffSpell *other) {
		return new GeneralBuffSpell(other);
	}
}