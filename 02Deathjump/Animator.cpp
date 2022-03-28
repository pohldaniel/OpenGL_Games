#include "Animator.h"
#include <iostream>

Animator::Animator(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, unsigned yStart, unsigned xLength, float time, unsigned int& textureAtlas, unsigned int& currentFrame) {
	
	m_updateTime = time;
	m_spriteSheet = new Spritesheet(pictureFile, tileWidth, tileHeight, true, true, yStart, xLength);
	m_textureAtlas = m_spriteSheet->getAtlas();
	m_frameCount = m_spriteSheet->getTotalFrames();

	i_textureAtlas = &textureAtlas;
	i_currentFrame = &currentFrame;

	*i_textureAtlas = m_textureAtlas;
}

Animator::~Animator() {}

void Animator::create(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, unsigned yStart, unsigned xLength, float time, unsigned int& textureAtlas, unsigned int& currentFrame) {
	if (m_frameCount > 0)
		return;

	m_updateTime = time;
	m_spriteSheet = new Spritesheet(pictureFile, tileWidth, tileHeight, true, true, yStart, xLength);
	m_textureAtlas = m_spriteSheet->getAtlas();
	m_frameCount = m_spriteSheet->getTotalFrames();

	i_textureAtlas = &textureAtlas;
	i_currentFrame = &currentFrame;

	*i_textureAtlas = m_textureAtlas;
}

void Animator::create(Spritesheet* spriteSheet, float time, unsigned int& textureAtlas, unsigned int& currentFrame) {
	
	m_updateTime = time;
	m_spriteSheet = spriteSheet;
	m_frameCount = spriteSheet->getTotalFrames();
	m_textureAtlas = spriteSheet->getAtlas();

	i_textureAtlas = &textureAtlas;
	i_currentFrame = &currentFrame;

	*i_textureAtlas = m_textureAtlas;
}

void Animator::update(const float deltaTime) {
	m_elapsedTime += deltaTime;
	while (m_elapsedTime >= m_updateTime) {
		m_elapsedTime -= m_updateTime;
		updateAnimation();
	}
}

void Animator::updateAnimation() {
	if (++m_currentFrame > m_frameCount - 1)
		m_currentFrame = 0;
	*i_textureAtlas = m_textureAtlas;
	*i_currentFrame = m_currentFrame;
}

unsigned int Animator::getCurrentFrame() const {
	return m_currentFrame;
}

unsigned int Animator::getFrameCount() const {
	return m_frameCount;
}

void Animator::setCurrentFrame(const unsigned int& frame) {
	m_currentFrame = frame;
	*i_textureAtlas = m_textureAtlas;
	*i_currentFrame = m_currentFrame;
}

void Animator::setUpdateTime(const float& time) {
	m_updateTime = time;
}

void Animator::setTextureAtlas(const unsigned int& atlas) {
	m_textureAtlas = atlas;
}

unsigned int Animator::getTextureAtlas() const {
	return m_textureAtlas;
}

