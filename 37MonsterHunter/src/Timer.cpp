#include "Timer.h"

Timer::Timer() : m_elapsedTime(0.0f), m_updateTime(10.0f), m_repeate(true), m_activated(false), m_startOnce(false) {

}

Timer::Timer(Timer const& rhs) : OnTimerEnd(rhs.OnTimerEnd) {
	m_elapsedTime = rhs.m_elapsedTime;
	m_updateTime = rhs.m_updateTime;
	m_repeate = rhs.m_repeate;
	m_activated = rhs.m_activated;
	m_startOnce = rhs.m_startOnce;
}

Timer::Timer(Timer&& rhs) : OnTimerEnd(std::move(rhs.OnTimerEnd)) {
	m_elapsedTime = rhs.m_elapsedTime;
	m_updateTime = rhs.m_updateTime;
	m_repeate = rhs.m_repeate;
	m_activated = rhs.m_activated;
	m_startOnce = rhs.m_startOnce;
}

Timer& Timer::operator=(const Timer& rhs) {
	m_elapsedTime = rhs.m_elapsedTime;
	m_updateTime = rhs.m_updateTime;
	m_repeate = rhs.m_repeate;
	m_activated = rhs.m_activated;
	m_startOnce = rhs.m_startOnce;
	OnTimerEnd = rhs.OnTimerEnd;
	return *this;
}

Timer& Timer::operator=(Timer&& rhs) {
	m_elapsedTime = rhs.m_elapsedTime;
	m_updateTime = rhs.m_updateTime;
	m_repeate = rhs.m_repeate;
	m_activated = rhs.m_activated;
	m_startOnce = rhs.m_startOnce;
	OnTimerEnd = rhs.OnTimerEnd;
	return *this;
}

void Timer::start(unsigned int milli, bool repeat, bool starOnce) {

	if (!m_activated && !m_startOnce) {
		m_activated = true;
		m_updateTime = static_cast<float>(milli) / 1000.0f;
		m_repeate = repeat;
		m_startOnce = starOnce;
	}
}

const bool Timer::isActivated() const {
	return m_activated;
}

void Timer::update(const float dt) {

	if (!m_activated)
		return;

	m_elapsedTime += dt;

	while (m_elapsedTime > m_updateTime) {
		if (OnTimerEnd) {
			OnTimerEnd();
		}
		m_elapsedTime = m_repeate ? m_elapsedTime - m_updateTime : 0.0f;
		m_activated = m_repeate;
	}
}

void Timer::stop() {
	if (m_activated) {
		m_activated = false;
		//OnTimerEnd = nullptr;
	}
}

void Timer::setOnTimerEnd(std::function<void()> fun) {
	OnTimerEnd = fun;
}