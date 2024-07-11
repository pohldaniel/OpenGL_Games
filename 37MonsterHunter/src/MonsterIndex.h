#pragma once

#include <engine/Sprite.h>

class MonsterIndex {

public:

	MonsterIndex();
	~MonsterIndex();

	void draw();
	void setViewWidth(float viewWidth);
	void setViewHeight(float viewHeight);

private:

	int m_visibleItems;	
	std::vector<std::pair<std::string, unsigned int>> m_names;
	Sprite m_surface;
	float m_viewWidth, m_viewHeight;
	unsigned int m_spritesheet;
};