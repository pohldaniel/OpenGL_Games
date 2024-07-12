#pragma once

#include <engine/Sprite.h>

enum Edge {
	ALL,
	BOTTOM_LEFT,
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_RIGHT,
	EDGE_NONE
};

class MonsterIndex {

public:

	MonsterIndex();
	~MonsterIndex();

	void draw();
	void processInput();
	void setViewWidth(float viewWidth);
	void setViewHeight(float viewHeight);

private:

	int m_visibleItems;	
	std::vector<std::tuple<std::string, unsigned int, bool>> m_names;
	Sprite m_surface;
	float m_viewWidth, m_viewHeight;
	unsigned int m_spritesheet;
	int m_currentOffset, m_currentSelected, m_beforeSelected;
};