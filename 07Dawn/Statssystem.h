#pragma once
class Character;

namespace Enums {	
	enum ElementType;
}

class StatsSystem {
private:
	// thought as a singleton. created by getStatsSystem
	StatsSystem();
	void calculateTables(int maxLevel, double relativeIncreasePerLevel);

	int maxLevel;
	int *maxStatsPointsForLevel;
	int *maxArmorPointsForLevel;
	double maxDamageModifier;
	double maxHitChance;
	double maxEvadeChance;
	double maxParryChance;
	double maxDamageReductionModifier;
	double maxBlockChance;
	double maxMeleeCriticalChance;
	double maxResistElementChance;
	double maxSpellEffectElementModifier;
	double maxSpellCriticalChance;

public:
	~StatsSystem();

	static const StatsSystem *getStatsSystem();

	double complexGetDamageModifier(int myLevel, int myDamageModifierPoints, int opponentLevel) const;
	double complexGetHitChance(int myLevel, int myHitModifierPoints, int opponentLevel) const;
	double complexGetEvadeChance(int myLevel, int myEvadeModifierPoints, int opponentLevel) const;
	double complexGetParryChance(int myLevel, int myParryModifierPoints, int opponentLevel) const;
	double complexGetDamageReductionModifier(int myLevel, int myDamageReductionPoints, int opponentLevel) const;
	double complexGetBlockChance(int myLevel, int myBlockModifierPoints, int opponentLevel) const;
	double complexGetMeleeCriticalStrikeChance(int myLevel, int myMeleeCriticalModifierPoints, int opponentLevel) const;
	double complexGetResistElementChance(int myLevel, int myResistElementModifierPoints, int opponentLevel) const;
	double complexGetSpellEffectElementModifier(int myLevel, int mySpellEffectElementModifierPoints, int opponentLevel) const;
	double complexGetSpellCriticalStrikeChance(int myLevel, int mySpellCriticalModifierPoints, int opponentLevel) const;

	int calculateDamageModifierPoints(const Character *character) const;
	int calculateHitModifierPoints(const Character *character) const;
	int calculateEvadeModifierPoints(const Character *character) const;
	int calculateParryModifierPoints(const Character *character) const;
	int calculateDamageReductionPoints(const Character *character) const;
	int calculateBlockModifierPoints(const Character *character) const;
	int calculateMeleeCriticalModifierPoints(const Character *character) const;
	int calculateResistElementModifierPoints(Enums::ElementType elementType, const Character *character) const;
	int calculateSpellEffectElementModifierPoints(Enums::ElementType elementType, const Character *character) const;
	int calculateSpellCriticalModifierPoints(const Character *character) const;
};