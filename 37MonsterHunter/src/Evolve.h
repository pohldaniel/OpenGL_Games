#pragma once
#include <functional>
#include <engine/Sprite.h>
#include <UI/Empty.h>

#include "Fade.h"
#include "Timer.h"

class Evolve : public ui::Empty {

public:

	Evolve();
	~Evolve();

	void draw() override;

	void update(float dt);
	void setViewWidth(float viewWidth);
	void setViewHeight(float viewHeight);
	void setCurrentMonster(std::string currentMonster);
	void setStartMonster(std::string startMonster);
	void setEndMonster(const std::string& endMonster);
	void startEvolution();
	void setOnEvolveEnd(std::function<void()> fun);
	void setCurentMonsterIndex(int curentMonsterIndex);
	void initUI(float viewWidth, float viewHeight);

private:

	float m_viewWidth, m_viewHeight, m_fadeValue;
	Sprite m_surface;
	std::string m_currentMonster, m_startMonster, m_endMonster;
	float m_elapsedTime, m_progress;
	int m_currentFrame;
	int m_frameCount, m_curentMonsterIndex;
	unsigned int m_atlasMonster;
	bool m_displayStar;
	Fade m_fade;
	Timer m_exitTimer;
	bool m_rotate;

	std::function<void()> OnEvolveEnd;
};