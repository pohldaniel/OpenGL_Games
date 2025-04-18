#include <memory>
#include <limits>
#include <cmath>
#include <cassert>

#include "Statssystem.h"
#include "Character.h"
#include "Enums.h"

std::auto_ptr<StatsSystem> theStatsSystem;

const StatsSystem *StatsSystem::getStatsSystem() {
	if (theStatsSystem.get() == NULL) {
		theStatsSystem = std::auto_ptr<StatsSystem>(new StatsSystem());
		theStatsSystem->calculateTables(99, 1.1);
	}

	return theStatsSystem.get();
}

StatsSystem::StatsSystem() {
	maxStatsPointsForLevel = NULL;
	maxArmorPointsForLevel = NULL;
}

StatsSystem::~StatsSystem() {
	if (maxStatsPointsForLevel != NULL) {
		delete[] maxStatsPointsForLevel;
	}

	if (maxArmorPointsForLevel != NULL) {
		delete[] maxArmorPointsForLevel;
	}
}

void StatsSystem::calculateTables(int maxLevel, double relativeIncreasePerLevel) {
	if (maxStatsPointsForLevel != NULL) {
		assert(maxArmorPointsForLevel != NULL);
		return;
	}

	assert(maxLevel > 1);

	this->maxLevel = maxLevel;

	// calculate the stats points needed for each level
	maxStatsPointsForLevel = new int[maxLevel + 1];
	maxArmorPointsForLevel = new int[maxLevel + 1];

	maxStatsPointsForLevel[0] = 100;
	maxArmorPointsForLevel[0] = 300;

	for (unsigned short curLevel = 1; curLevel <= maxLevel; ++curLevel) {
		maxStatsPointsForLevel[curLevel] = maxStatsPointsForLevel[0] * pow(relativeIncreasePerLevel, curLevel - 1);
		maxArmorPointsForLevel[curLevel] = maxArmorPointsForLevel[0] * pow(relativeIncreasePerLevel, curLevel - 1);
	}

	// set the allowed maximal percentages for different attributes
	maxDamageModifier = std::numeric_limits<double>::max();
	maxHitChance = 1.0;
	maxEvadeChance = 0.5;
	maxParryChance = 0.5;
	maxDamageReductionModifier = 0.75;
	maxBlockChance = 0.6;
	maxMeleeCriticalChance = 0.5;
	maxResistElementChance = 0.75;
	maxSpellEffectElementModifier = std::numeric_limits<double>::max();
	maxSpellCriticalChance = 0.5;
}

static double getModifierCommon(int myModifierPoints, int maxModifierPointsOwn, int maxModifierPointsOpponent, double maxModifierValue) {
	int usePoints = myModifierPoints;

	if (usePoints > maxModifierPointsOwn) {
		usePoints = maxModifierPointsOwn;
	}

	if (usePoints > maxModifierPointsOpponent) {
		usePoints = maxModifierPointsOpponent;
	}

	double result = (static_cast<double>(usePoints) / static_cast<double>(maxModifierPointsOpponent));

	if (result > maxModifierValue) {
		return maxModifierValue;
	}else {
		return result;
	}
}

double StatsSystem::complexGetDamageModifier(int myLevel, int myDamageModifierPoints, int opponentLevel) const {
	return 1 + (static_cast<double>(myDamageModifierPoints) / 100.0);
}

double StatsSystem::complexGetHitChance(int myLevel, int myHitModifierPoints, int opponentLevel) const {
	assert(myLevel <= maxLevel && opponentLevel <= maxLevel);

	return getModifierCommon(myHitModifierPoints,
		maxStatsPointsForLevel[myLevel],
		maxStatsPointsForLevel[opponentLevel],
		maxHitChance);
}

double StatsSystem::complexGetEvadeChance(int myLevel, int myEvadeModifierPoints, int opponentLevel) const {
	assert(myLevel <= maxLevel && opponentLevel <= maxLevel);

	return getModifierCommon(myEvadeModifierPoints,
		maxStatsPointsForLevel[myLevel],
		maxStatsPointsForLevel[opponentLevel],
		maxEvadeChance);
}

double StatsSystem::complexGetParryChance(int myLevel, int myParryModifierPoints, int opponentLevel) const {
	assert(myLevel <= maxLevel && opponentLevel <= maxLevel);

	return getModifierCommon(myParryModifierPoints,
		maxStatsPointsForLevel[myLevel],
		maxStatsPointsForLevel[opponentLevel],
		maxParryChance);
}

double StatsSystem::complexGetDamageReductionModifier(int myLevel, int myDamageReductionPoints, int opponentLevel) const {
	assert(myLevel <= maxLevel && opponentLevel <= maxLevel);

	return getModifierCommon(myDamageReductionPoints,
		maxArmorPointsForLevel[myLevel],
		maxArmorPointsForLevel[opponentLevel],
		maxDamageReductionModifier);
}

double StatsSystem::complexGetBlockChance(int myLevel, int myBlockModifierPoints, int opponentLevel) const {
	assert(myLevel <= maxLevel && opponentLevel <= maxLevel);

	return getModifierCommon(myBlockModifierPoints,
		maxStatsPointsForLevel[myLevel],
		maxStatsPointsForLevel[opponentLevel],
		maxBlockChance);
}

double StatsSystem::complexGetMeleeCriticalStrikeChance(int myLevel, int myMeleeCriticalModifierPoints, int opponentLevel) const {
	assert(myLevel <= maxLevel && opponentLevel <= maxLevel);

	return getModifierCommon(myMeleeCriticalModifierPoints,
		maxStatsPointsForLevel[myLevel],
		maxStatsPointsForLevel[opponentLevel],
		maxMeleeCriticalChance);
}

double StatsSystem::complexGetResistElementChance(int myLevel, int myResistElementModifierPoints, int opponentLevel) const {
	assert(myLevel <= maxLevel && opponentLevel <= maxLevel);

	return getModifierCommon(myResistElementModifierPoints,
		maxStatsPointsForLevel[myLevel],
		maxStatsPointsForLevel[opponentLevel],
		maxResistElementChance);
}

double StatsSystem::complexGetSpellEffectElementModifier(int myLevel, int mySpellEffectElementModifierPoints, int opponentLevel) const {
	assert(myLevel <= maxLevel && opponentLevel <= maxLevel);

	return 1 + (static_cast<double>(mySpellEffectElementModifierPoints) / 100.0);
}

double StatsSystem::complexGetSpellCriticalStrikeChance(int myLevel, int mySpellCriticalModifierPoints, int opponentLevel) const {
	assert(myLevel <= maxLevel && opponentLevel <= maxLevel);

	return getModifierCommon(mySpellCriticalModifierPoints,
		maxStatsPointsForLevel[myLevel],
		maxStatsPointsForLevel[opponentLevel],
		maxSpellCriticalChance);
}

int StatsSystem::calculateDamageModifierPoints(const Character *character) const {
	return (character->getModifiedStrength());
}

int StatsSystem::calculateHitModifierPoints(const Character *character) const {
	return (character->getModifiedDexterity() * 4);
}

int StatsSystem::calculateEvadeModifierPoints(const Character *character) const {
	return (character->getModifiedDexterity() / 4);
}

int StatsSystem::calculateParryModifierPoints(const Character *character) const {
	return (character->getModifiedDexterity() / 4);
}

int StatsSystem::calculateDamageReductionPoints(const Character *character) const {
	return 0;
}

int StatsSystem::calculateBlockModifierPoints(const Character *character) const {
	return ((character->getModifiedDexterity() + character->getModifiedStrength()) / 5);
}

int StatsSystem::calculateMeleeCriticalModifierPoints(const Character *character) const {
	return ((character->getModifiedIntellect() + character->getModifiedWisdom()) / 5);
}

int StatsSystem::calculateResistElementModifierPoints(Enums::ElementType elementType, const Character *character) const {
	return character->getModifiedWisdom() / 4;
}

int StatsSystem::calculateSpellEffectElementModifierPoints(Enums::ElementType elementType, const Character *character) const {
	return character->getModifiedIntellect();
}

int StatsSystem::calculateSpellCriticalModifierPoints(const Character *character) const {
	return ((character->getModifiedIntellect() + character->getModifiedWisdom()) / 5);
}
