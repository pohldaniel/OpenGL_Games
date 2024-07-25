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
	void reduceEnergy(const AttackData& attack);
	void playAttackAnimation();
	void checkAttack();
	const bool getDisableAttack(size_t index) const;

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
	bool m_pause, m_highlight, m_coverWithMask, m_disableAttack;
	unsigned int m_attackOffset;
	Timer m_highlightTimer;

	static void CheckAttack(bool& disableAttack, const std::unordered_map<std::string, AttackData>& AttackData, const Monster& monster);
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