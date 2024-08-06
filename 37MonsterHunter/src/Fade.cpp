#include <iostream>
#include <algorithm>
#include "Fade.h"

Fade::Fade(float& fadeValue) : m_transitionSpeed(1.0f), m_fadeValue(fadeValue), m_transitionEnd(false), m_fadeIn(false), m_fadeOut(false), m_activate(false), m_activateIn(false), m_activateOut(false), OnFadeEnd(nullptr), OnFadeIn(nullptr), OnFadeOut(nullptr){

}

void Fade::update(const float dt) {
	if (m_fadeIn) {
		m_fadeValue = m_fadeValue <= 1.0f ? m_fadeValue + m_transitionSpeed * dt : 1.01f;		
		m_fadeIn = m_fadeValue <= 1.0f;
		m_transitionEnd = !m_fadeIn;	
		m_fadeValue = std::min(m_fadeValue, 1.0f);
	}

	if (m_fadeOut) {		
		m_fadeValue = m_fadeValue >= 0.0f ? m_fadeValue - m_transitionSpeed * dt : -0.01f;		
		m_fadeOut = m_fadeValue >= 0.0f;
		m_transitionEnd = m_fadeOut;
		m_fadeValue = std::max(m_fadeValue, 0.0f);
	}

	if (m_activate && OnFadeEnd && !m_fadeOut && !m_fadeIn) {
		m_activate = false;
		OnFadeEnd();
	}

	if (m_activateIn && OnFadeIn && !m_fadeOut && !m_fadeIn) {
		m_activateIn = false;
		OnFadeIn();
	}

	if (m_activateOut && OnFadeOut && !m_fadeOut && !m_fadeIn) {
		m_activateOut = false;
		OnFadeOut();
	}
}

void Fade::toggleFade(bool activate) {
	if (!m_transitionEnd) {
		m_fadeOut = m_fadeIn;
		m_fadeIn = !m_fadeIn;
	}else {
		m_fadeIn = m_fadeOut;
		m_fadeOut = !m_fadeOut;
	}
	m_activate = activate;
}

void Fade::fadeIn(bool activate) {
	if (m_fadeIn)
		return;

	m_fadeIn = true;
	m_fadeOut = false;
	m_fadeValue = 0.0f;
	m_activateIn = activate;
}

void Fade::fadeOut(bool activate) {
	if (m_fadeOut)
		return;

	m_fadeIn = false;
	m_fadeOut = true;
	m_fadeValue = 1.0f;
	m_activateOut = activate;
}

void Fade::setTransitionSpeed(const float transitionSpeed) {
	m_transitionSpeed = transitionSpeed;
}

void Fade::setOnFadeEnd(std::function<void()> fun) {
	OnFadeEnd = fun;
}

void Fade::setOnFadeIn(std::function<void()> fun) {
	OnFadeIn = fun;
}

void Fade::setOnFadeOut(std::function<void()> fun) {
	OnFadeOut = fun;
}

void Fade::setTransitionEnd(bool transitionEnd) {
	m_transitionEnd = transitionEnd;
}

void Fade::setFadeValue(float fadeValue) {
	m_fadeValue = fadeValue;
}

const bool Fade::isActivated() const {
	return m_activate || m_activateIn || m_activateOut;
}