#pragma once

#include <engine/Sprite.h>

enum Edge {
	ALL,
	BOTTOM_LEFT,
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_RIGHT,
	EDGE_RIGHT,
	EDGE_LEFT,
	EDGE_NONE
};

struct Monster {
	std::string name;
	unsigned int level;
	bool selected;
};

struct MonsterData {
	std::string element;
	unsigned int maxHealth;
	unsigned int maxEnergy;
	float attack;
	float defense;
	float recovery;
	float speed;
	unsigned int graphic;
	std::unordered_map <std::string, unsigned int> abilities;
	std::tuple<std::string, unsigned int> evolve;
};

class MonsterIndex {

public:

	MonsterIndex();
	~MonsterIndex();

	void draw();
	void update(float dt);
	void processInput();
	void setViewWidth(float viewWidth);
	void setViewHeight(float viewHeight);

	void drawBar(const Rect& rect, float value, float maxValue, const Vector4f& bgColor, const Vector4f& color, float radius = 1.0f);

private:

	void resetAnimation();

	int m_visibleItems;	
	std::vector<Monster> m_monster;
	Sprite m_surface;
	Sprite m_surfaceBar;
	float m_viewWidth, m_viewHeight;
	unsigned int m_atlasIcons, m_atlasMonster;
	int m_currentOffset, m_currentSelected, m_beforeSelected;

	std::unordered_map<std::string, Vector4f> m_colorMap;
	std::unordered_map<std::string, MonsterData> m_monsterData;

	float m_elapsedTime;
	int m_currentFrame;
	int m_frameCount;
};