#pragma once
#include <ordered_map.h>
#include <engine/Sprite.h>
#include <UI/WidgetMH.h>
#include <UI/Label.h>
#include <UI/Surface.h>
#include <UI/Icon.h>
#include <UI/Bar.h>

struct MonsterEntry {
	std::string name;
	unsigned int level;
	float health;
	float energy;
	float experience;
	bool selected;

	void resetStats();
	void unselect();
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

class MonsterIndex : public Surface {

public:

	MonsterIndex();
	~MonsterIndex();

	void draw() override;

	void update(float dt);
	void processInput();
	void setViewWidth(float viewWidth);
	void setViewHeight(float viewHeight);
	void resetStats();
	void addAbilities();
	void eraseAbilities();
	void addMonsters();
	void eraseMonsters();
	void resetAnimation();
	void unselect();
	void reset();
	void initUI(float viewWidth, float viewHeight);

	static std::unordered_map<std::string, MonsterData> MonsterData;
	static std::vector<MonsterEntry> Monsters;
	static std::unordered_map<std::string, AttackData> _AttackData;
	static std::unordered_map<std::string, Vector4f> ColorMap;

private:

	
	int m_visibleItems;	

	std::vector<std::string> m_stats;
	Sprite m_surface;
	
	float m_viewWidth, m_viewHeight;
	unsigned int m_atlasIcons, m_atlasMonster;
	int m_currentOffset, m_currentSelected, m_beforeSelected;

	float m_elapsedTime;
	int m_currentFrame;
	int m_frameCount;
	std::string m_stateLabels[6];
	std::unordered_map<std::string, float> m_maxStats;

	
	bool m_rotate;
};