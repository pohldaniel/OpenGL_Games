#pragma once

#include <engine/Sprite.h>

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
	std::vector<std::tuple<std::string, unsigned int, Vector4f>> m_names;
	Sprite m_surface;
	float m_viewWidth, m_viewHeight;
	unsigned int m_spritesheet;
	int m_currentOffset, m_currentSelected;
};