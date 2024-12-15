#pragma once
#include <random>
#include <Timer.h>
#include <Entities/SpriteEntity.h>
#include <UI/Surface.h>

#include "MonsterIndex.h"

class Monster : public SpriteEntity, public ui::Surface {

public:

	Monster(Cell& cell, MonsterEntry& monsterEntry, const Vector2f& pos);
	Monster& operator=(const Monster& rhs);
	Monster& operator=(Monster&& rhs);
	Monster(Monster const& rhs);
	Monster(Monster&& rhs);

	virtual ~Monster();

	void update(float dt) override;
	float getInitiative();
	void setInitiative(float initiative);
	void pause();
	void unPause();
	void setHighlight(bool highlight);
	void setHighlight(bool highlight, unsigned int milli);
	const std::string& getName() const;
	const unsigned int getLevel() const;
	const float getEnergy() const;
	const float getHealth() const;
	const float getMaxHealth() const;
	const float getExperience() const;
	void reduceEnergy(const AttackData& attack);
	void playAttackAnimation();
	void canAttack();
	void setCanAttack(bool canAttack);
	const bool getCanAttack() const;
	void applyAttack(float amount, const AttackData& attackData);
	const float getBaseDamage(const std::string attackName) const;
	void setDefending(bool defending);
	const bool getDelayedKill() const;
	const bool getKilled() const;
	void setKilled(bool killed);
	void startDelayedKill();
	const bool getPause() const;
	void updateExperience(float amount);
	void showMissing();
	void OnCallBack(CallBack callback);
	void setGraphic(int graphic);
	void setDelayedKill(bool delayedKill);

	MonsterEntry& getMonsterEntry();
	void setMonsterEntry(MonsterEntry& monsterEntry);
	void calculateStates(MonsterEntry& monsterEntry);
	
private:

	void drawDefault() override;
	void initUI();

	std::reference_wrapper<MonsterEntry> monsterEntry;

	float m_animationSpeed;
	float m_maxHealth, m_maxEnergy, m_maxExperience;
	float m_initiative;
	float m_speed;
	bool m_pause, m_highlight, m_coverWithMask, m_canAttack, m_defending, m_delayedKill, m_killed;
	unsigned int m_attackOffset;
	CBTimer<Monster> m_highlightTimer;
	CBTimer<Monster> m_delayedKillTimer;
	CBTimer<Monster> m_showMissingTimer;

	static void CanAttack(bool& canAttack, const std::unordered_map<std::string, AttackData>& AttackData, const Monster& monster);
	static float GetBaseDamage(const std::string monsterName, const std::string attackName, unsigned int level);

	static std::random_device RandomDevice;
	static std::uniform_real_distribution<float> Distribution;
};

struct MonsterFunctor {

	MonsterFunctor(void (Monster::*f)(bool, unsigned int), bool _placeholder1, unsigned int _placeholder2) : setHighlight(f), placeholder1(_placeholder1), placeholder2(_placeholder2) {}

	void (Monster::*setHighlight)(bool, unsigned int);
	bool placeholder1;
	unsigned int placeholder2;
	void operator()(Monster& i) { (i.*setHighlight)(placeholder1, placeholder2); }
};