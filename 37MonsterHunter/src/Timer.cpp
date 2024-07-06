#include <iostream>
#include "Timer.h"

Timer::Timer() : m_elapsedTime(0.0f), m_updateTime(10.0f), m_repeate(true), m_activated(false), OnTimerEnd(nullptr){

}

void Timer::update(const float dt) {
	if (!m_activated)
		return;

	m_elapsedTime +=  dt;

	while (m_elapsedTime > m_updateTime) {
		if(OnTimerEnd)
			OnTimerEnd();
		m_elapsedTime = m_repeate ? m_elapsedTime - m_updateTime : 0.0f;
		m_activated = m_repeate;
	}
}

void Timer::setOnTimerEnd(std::function<void()> fun) {
	OnTimerEnd = fun;
}

void Timer::start(unsigned int milli, bool repeat) {
	m_activated = true;
	m_updateTime = static_cast<float>(milli) / 1000.0f;
	m_repeate = repeat;
}

void Timer::stop() {
	m_activated = false;
	OnTimerEnd = nullptr;
}