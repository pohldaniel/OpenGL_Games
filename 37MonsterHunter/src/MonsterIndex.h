#pragma once
#include<ordered_map.h>
#include <engine/Sprite.h>

enum Edge {
	ALL,
	BOTTOM_LEFT,
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_RIGHT,
	EDGE_RIGHT,
	EDGE_LEFT,
	TOP,
	BOTTOM,
	EDGE_NONE
};

struct MonsterEntry {
	std::string name;
	unsigned int level;
	float health;
	float energy;
	float experience;
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
	tsl::ordered_map<std::string, unsigned int> abilities;
	std::tuple<std::string, unsigned int> evolve;
};

struct AttackData {
	std::string target;
	float amount;
	float cost;
	std::string element;
	std::string animation;
	unsigned int graphic;
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

	void drawBar(const Rect& rect, const TextureRect& textureRect, float value, float maxValue, const Vector4f& bgColor, const Vector4f& color);

	static void DrawBar(const Rect& rect, float value, float maxValue, const Vector4f& bgColor, const Vector4f& color, float radius = 1.0f);

	static std::unordered_map<std::string, MonsterData> MonsterData;
	static std::vector<MonsterEntry> Monsters;
	static std::unordered_map<std::string, AttackData> _AttackData;
	static std::unordered_map<std::string, Vector4f> ColorMap;

private:

	void resetAnimation();
	int m_visibleItems;	

	std::vector<std::string> m_stats;
	Sprite m_surface;
	
	float m_viewWidth, m_viewHeight;
	unsigned int m_atlasIcons, m_atlasMonster;
	int m_currentOffset, m_currentSelected, m_beforeSelected;

	float m_elapsedTime;
	int m_currentFrame;
	int m_frameCount;

	std::unordered_map<std::string, float> m_maxStats;

	static Sprite SurfaceBar;
};