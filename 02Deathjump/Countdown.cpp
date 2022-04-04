#include <iostream>
#include "Countdown.h"

Countdown::Countdown() {
	initCounter();
	m_effectsPlayer.init();
	m_effectsPlayer.setVolume(Globals::soundVolume * 0.3f);
	m_effectsPlayer.play(Globals::soundManager.get("3").getBuffer());
}

Countdown::~Countdown() {
	delete m_counter;
}

void Countdown::update() {
	if (m_currentFrame > 4)
		return;
	if (clock.getElapsedTimeSec() > 1.0f) {
		m_currentFrame++;
		if (m_currentFrame < 4) {
			m_counter->calcSize(str[m_currentFrame]);
			m_counter->setOrigin(m_counter->getSize()[0] * 0.5f, m_counter->getSize()[1]);
			switch (m_currentFrame){
			case 1:
				m_effectsPlayer.play(Globals::soundManager.get("2").getBuffer());				
				break;
			case 2:
				m_effectsPlayer.play(Globals::soundManager.get("1").getBuffer());				
				break;
			case 3:
				m_effectsPlayer.play(Globals::soundManager.get("go").getBuffer());				
				break;
			}
		}	
		this->clock.restart();
	}
}

unsigned Countdown::currentFrame() const {
	return m_currentFrame;
}

void Countdown::initCounter() {
	clock.restart();
	m_counter = new Text(3, Globals::fontManager.get("font_200"), 140.0f / 200.0f);
	m_counter->setPosition(WIDTH * 0.5f, HEIGHT - 35.0f);

	m_counter->calcSize(str[m_currentFrame]);
	m_counter->setOrigin(m_counter->getSize()[0] * 0.5f, m_counter->getSize()[1]);
}

void Countdown::render() {
	if (m_currentFrame < 4)
      m_counter->render(str[m_currentFrame]);			
}
