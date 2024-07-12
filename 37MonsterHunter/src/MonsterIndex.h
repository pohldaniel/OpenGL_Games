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
	std::vector<std::tuple<std::string, unsigned int, unsigned int, bool, std::string, unsigned int>> m_names;
	Sprite m_surface;
	Sprite m_surfaceBar;
	float m_viewWidth, m_viewHeight;
	unsigned int m_atlasIcons, m_atlasMonster;
	int m_currentOffset, m_currentSelected, m_beforeSelected;

	std::unordered_map<std::string, Vector4f> m_colorMap;

	float m_elapsedTime;
	int m_currentFrame;
	int m_frameCount;
};