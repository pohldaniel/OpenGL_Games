#include <iostream>
#include "Timer.h"

Timer::Timer() : m_elapsedTime(0.0f), m_delayTime(0.0f), m_updateTime(10.0f), m_repeate(true), m_activated(false), m_startOnce(false) {
	OnUpdate = std::bind(&Timer::updateDefault, this, std::placeholders::_1);
}

Timer::Timer(Timer const& rhs) : OnTimerEnd(rhs.OnTimerEnd), OnUpdate(rhs.OnUpdate){
	m_elapsedTime = rhs.m_elapsedTime;
	m_updateTime = rhs.m_updateTime;
	m_delayTime = rhs.m_delayTime;
	m_repeate = rhs.m_repeate;
	m_activated = rhs.m_activated;
	m_startOnce = rhs.m_startOnce;
}

Timer::Timer(Timer&& rhs)  noexcept : OnTimerEnd(std::move(rhs.OnTimerEnd)), OnUpdate(std::move(rhs.OnUpdate)) {
	m_elapsedTime = rhs.m_elapsedTime;
	m_updateTime = rhs.m_updateTime;
	m_delayTime = rhs.m_delayTime;
	m_repeate = rhs.m_repeate;
	m_activated = rhs.m_activated;
	m_startOnce = rhs.m_startOnce;
}

Timer& Timer::operator=(const Timer& rhs) {
	m_elapsedTime = rhs.m_elapsedTime;
	m_updateTime = rhs.m_updateTime;
	m_delayTime = rhs.m_delayTime;
	m_repeate = rhs.m_repeate;
	m_activated = rhs.m_activated;
	m_startOnce = rhs.m_startOnce;
	OnTimerEnd = rhs.OnTimerEnd;
	OnUpdate = rhs.OnUpdate;
	return *this;
}

Timer& Timer::operator=(Timer&& rhs) noexcept {
	m_elapsedTime = rhs.m_elapsedTime;
	m_updateTime = rhs.m_updateTime;
	m_delayTime = rhs.m_delayTime;
	m_repeate = rhs.m_repeate;
	m_activated = rhs.m_activated;
	m_startOnce = rhs.m_startOnce;
	OnTimerEnd = std::move(rhs.OnTimerEnd);
	OnUpdate = std::move(rhs.OnUpdate);
	return *this;
}

void Timer::start(unsigned int milli, bool repeat, unsigned int delayMilli, bool starOnce) {

	if (!m_activated && !m_startOnce) {
		m_activated = true;
		m_updateTime = static_cast<float>(milli) / 1000.0f;
		m_delayTime = static_cast<float>(delayMilli) / 1000.0f;
		m_repeate = repeat;
		m_startOnce = starOnce;
	}
}

void Timer::startExclusive(unsigned int milli, bool repeat, unsigned int delayMilli, bool starOnce) {

	if (!m_activated && !m_startOnce && m_elapsedTime == 0.0f) {
		m_activated = true;
		m_updateTime = static_cast<float>(milli) / 1000.0f;
		m_delayTime = static_cast<float>(delayMilli) / 1000.0f;
		m_repeate = repeat;
		m_startOnce = starOnce;
	}
}

const bool Timer::isActivated() const {
	return m_activated;
}

void Timer::update(const float dt) {
	OnUpdate(dt);
}

void Timer::updateDefault(const float dt) {
	if (!m_activated)
		return;

	m_elapsedTime += dt;

	while (m_elapsedTime > m_updateTime) {
		m_elapsedTime = m_repeate ? m_elapsedTime - m_updateTime : m_delayTime > 0.0f ? m_elapsedTime : 0.0f;
		m_activated = m_repeate;

		if (OnTimerEnd) {
			OnTimerEnd();
		}

		if (m_elapsedTime < m_updateTime + m_delayTime && m_delayTime > 0.0f) {
			OnUpdate = std::bind(&Timer::updateDelay, this, std::placeholders::_1);
			break;
		}
	}
}

void Timer::updateDelay(const float dt) {
	m_elapsedTime += dt;
	while (m_elapsedTime > m_updateTime + m_delayTime) {
		m_elapsedTime = 0.0f;
		OnUpdate = std::bind(&Timer::updateDefault, this, std::placeholders::_1);
	}
}

void Timer::stop() {
	if (m_activated) {
		m_activated = false;
		m_elapsedTime = 0.0f;
		//OnTimerEnd = nullptr;
	}
}

void Timer::setOnTimerEnd(std::function<void()> fun) {
	OnTimerEnd = fun;
}

void Timer::setStartOnce(bool startOnce){
	m_startOnce = startOnce;
}

void Timer::print() {
	std::cout << "Activated: " << m_activated << std::endl;
	std::cout << "Elapsed Time: " << m_elapsedTime << std::endl;
	std::cout << "Update Time: " << m_updateTime << std::endl;
	std::cout << "Repeat: " << m_repeate << std::endl;
	std::cout << "Start Once: " << m_startOnce << std::endl;
	std::cout << "--------------------" << std::endl;
}