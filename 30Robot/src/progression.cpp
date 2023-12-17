#include <Event/EventListener.h>
#include <Event/EventEmitter.h>

#include "progression.hpp"

Progression::Progression(EventEmitter& emitter)
	: m_emitter(emitter), m_introImgPath(""), m_introText(""), m_exitText(""),
	m_maxLife(0), m_life(0), m_mirrorNumber(0), m_slowNumber(0)
{}

Progression::~Progression() {}

void Progression::resetValues() {
	m_introText = "";
	m_exitText = "";
	m_life = 0; 
	m_mirrorNumber = 0;
	m_slowNumber = 0;
	m_emitter.publish<evnt::ProgressionUpdated>();
}

void Progression::setIntroImgPath(std::string imgPath) {
	m_introImgPath = imgPath;
	m_emitter.publish<evnt::ProgressionUpdated>();
}
const char* Progression::getIntroImgPath() const {
	return m_introImgPath.c_str();
}

void Progression::setIntroText(std::string text) {
	m_introText = text;
	m_emitter.publish<evnt::ProgressionUpdated>();
}
const char* Progression::getIntroText() const {
	return m_introText.c_str();
}

void Progression::setExitText(std::string text) {
	m_exitText = text;
	m_emitter.publish<evnt::ProgressionUpdated>();
}
const char* Progression::getExitText() const {
	return m_exitText.c_str();
}

void Progression::setMaxLife(int value) {
	m_maxLife = value;
	m_emitter.publish<evnt::ProgressionUpdated>();
}
int Progression::getMaxLife() {
	return m_maxLife;
}

void Progression::setLife(int value) {
	m_life = value;
	m_emitter.publish<evnt::ProgressionUpdated>();
}
void Progression::reduceLifeBy(int value) {
	m_life -= value;
	m_emitter.publish<evnt::ProgressionUpdated>();
	if (m_life <= 0) {
		m_emitter.publish<evnt::Loose>();
	}
}
int Progression::getLife() const {
	return m_life;
}

void Progression::setMirrorNumber(int value) {
	m_mirrorNumber = value;
	m_emitter.publish<evnt::ProgressionUpdated>();
}
void Progression::reduceMirrorNumberBy1() {
	if (m_mirrorNumber >= 0) {
		m_mirrorNumber--;
		m_emitter.publish<evnt::ProgressionUpdated>();
	}
}
void Progression::increaseMirrorNumberBy1() {
	m_mirrorNumber++;
	m_emitter.publish<evnt::ProgressionUpdated>();
}
int Progression::getMirrorNumbers() const {
	return m_mirrorNumber;
}

void Progression::setSlowNumber(int value) {
	m_slowNumber = value;
}
void Progression::reduceSlowNumberBy1() {
	if (m_slowNumber >= 0) {
		m_slowNumber--;
		m_emitter.publish<evnt::ProgressionUpdated>();
	}
}
void Progression::increaseSlowNumberBy1() {
	m_slowNumber++;
	m_emitter.publish<evnt::ProgressionUpdated>();
}
int Progression::getSlowNumbers() const {
	return m_slowNumber;
}

int Progression::getRobotNumber() {
	return m_robotNumber;
}
void Progression::setRobotNumber(int value) {
	m_robotNumber = value;
}
void Progression::decreaseRobotNumber() {
	m_robotNumber -= 1;
}

int Progression::getKamikazeNumber() {
	return m_kamikazeNumber;
}
void Progression::setKamikazeNumber(int value) {
	m_kamikazeNumber = value;
}
void Progression::decreaseKamikazeNumber() {
	m_kamikazeNumber -= 1;
}

void Progression::setWaveRate(int value) {
	m_waveRate = value;
}
int Progression::getWaveRate() {
	return m_waveRate;
}

void Progression::setLevelNumber(int value) {
	m_levelNumber = value;
}
int Progression::getLevelNumber() const {
	return m_levelNumber;
}