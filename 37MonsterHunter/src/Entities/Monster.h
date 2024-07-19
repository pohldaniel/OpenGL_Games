#pragma once
#include <random>
#include <Timer.h>
#include <Entities/SpriteEntity.h>


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
	bool m_pause, m_highlight, m_coverWithMask;

	Timer m_highlightTimer;

	static std::random_device RandomDevice;
	static std::uniform_real_distribution<float> Distribution;
};