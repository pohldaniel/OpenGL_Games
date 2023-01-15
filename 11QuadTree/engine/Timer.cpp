#include "Timer.h"

void Timer::update(const float& dt) {
	m_elapsedTime += dt;
	while (m_elapsedTime > m_updateTime) {
		m_fun();
		m_elapsedTime -= m_updateTime;
	}
}

void Timer::setFunction(float time, std::function<void()> fun) {
	m_updateTime = time;
	m_fun = fun;
}

void Timer::setUpdateTime(float time) {
	m_updateTime = time;
}

float Timer::getUpdateTime() const {
	return m_updateTime;
}
