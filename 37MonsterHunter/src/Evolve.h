#pragma once
#include <functional>
#include <engine/Sprite.h>
#include "Fade.h"
#include "Timer.h"
class Evolve {

public:

	Evolve();
	~Evolve();

	void draw();
	void update(float dt);
	void setViewWidth(float viewWidth);
	void setViewHeight(float viewHeight);
	void setCurrentMonster(std::string currentMonster);
	void setNextMonster(const std::string& nextMonster);
	void startEvolution();
	void setOnEvolveEnd(std::function<void()> fun);
private:

	float m_viewWidth, m_viewHeight, m_fadeValue;
	Sprite m_surface;
	std::string m_currentMonster, m_nextMonster;

	float m_elapsedTime;
	int m_currentFrame;
	int m_frameCount;
	unsigned int m_atlasMonster;

	Fade m_fade;
	Timer m_exitTimer;

	std::function<void()> OnEvolveEnd;
};