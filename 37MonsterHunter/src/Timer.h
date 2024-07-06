#pragma once
#include <functional>

class Timer {

public:

	Timer();

	void update(const float dt);	
	void start(unsigned int milli, bool repeat = true);
	void setOnTimerEnd(std::function<void()> fun);
	void stop();

private:

	std::function<void()> OnTimerEnd;
	float m_elapsedTime;
	float m_updateTime;
	bool m_repeate;
	bool m_activated;
};
