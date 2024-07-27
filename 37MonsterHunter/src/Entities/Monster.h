#pragma once
#include <random>
#include <Timer.h>
#include <Entities/SpriteEntity.h>

#include "MonsterIndex.h"

struct Bar {
	Rect rect;
	TextureRect textureRect;
	float value;
	float maxValue;
	Vector4f bgColor;
	Vector4f color;
};

class Monster : public SpriteEntity {

public:

	Monster(Cell& cell, std::string name, unsigned int level, float experience, float health, float energy);
	Monster& operator=(const Monster& rhs);
	Monster& operator=(Monster&& rhs);
	Monster(Monster const& rhs);
	Monster(Monster&& rhs);

	virtual ~Monster();

	void drawBack();
	void draw();
	void drawBars();
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
	void reduceEnergy(const AttackData& attack);
	void playAttackAnimation();
	void canAttack();
	void setCanAttack(bool canAttack);
	const bool getCanAttack() const;
	void applyAttack(float amount, unsigned int targetLevel, const AttackData& attackData);
	float getBaseDamage(const std::string attackName);
	void setIsDefending(bool isDefending);
	const bool getDelayedKill() const;
	void startDelayedKill();
	const bool getPause() const;

	void OnCallBack(CallBack callback);

private:

	void drawBar(const Rect& rect, const TextureRect& textureRect, float value, float maxValue, const Vector4f& bgColor, const Vector4f& color);
	void addBar(const Rect& rect, const TextureRect& textureRect, float value, float maxValue, const Vector4f& bgColor, const Vector4f& color);
	std::vector<Bar> m_bars;

	float m_animationSpeed;
	std::string m_name;
	unsigned int m_level;
	float m_experience, m_maxExperience;
	float m_health, m_maxHealth;
	float m_energy, m_maxEnergy;
	float m_initiative;
	float m_speed;
	bool m_pause, m_highlight, m_coverWithMask, m_canAttack, m_isDefending, m_delayedKill;
	unsigned int m_attackOffset;
	CBTimer<Monster> m_highlightTimer;
	CBTimer<Monster> m_delayedKillTimer;

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