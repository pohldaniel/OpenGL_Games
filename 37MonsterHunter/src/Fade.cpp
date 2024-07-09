#include <iostream>
#include "Fade.h"

Fade::Fade(float& fadeValue) :m_transitionSpeed(1.0f), m_fadeValue(fadeValue), m_transitionEnd(true), m_fadeIn(true), m_fadeOut(false), m_activate(false), OnFadeEnd(nullptr){

}

void Fade::update(const float dt) {
	if (m_fadeIn) {
		m_fadeValue = m_fadeValue <= 1.0f ? m_fadeValue + m_transitionSpeed * dt : 1.0f;
		m_fadeIn = m_fadeValue <= 1.0f;
		m_transitionEnd = !m_fadeIn;
	}

	if (m_fadeOut) {
		m_fadeValue = m_fadeValue >= 0.0f ? m_fadeValue - m_transitionSpeed * dt : 0.0f;
		m_fadeOut = m_fadeValue >= 0.0f;
		m_transitionEnd = m_fadeOut;
	}

	if (m_activate && OnFadeEnd) {
		m_activate = false;
		OnFadeEnd();
	}
}

void Fade::setOnFadeEnd(std::function<void()> fun) {
	OnFadeEnd = fun;
}

void Fade::toggleFade() {
	if (!m_transitionEnd) {
		m_fadeOut = m_fadeIn;
		m_fadeIn = !m_fadeIn;
	}else {
		m_fadeIn = m_fadeOut;
		m_fadeOut = !m_fadeOut;
	}
	m_activate = true;
}

void Fade::setTransitionSpeed(const float transitionSpeed) {
	m_transitionSpeed = transitionSpeed;
}