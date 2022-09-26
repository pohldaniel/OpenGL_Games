#include "Animation2D.h"
#include <iostream>

Animation2D::Animation2D(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, unsigned yStart, unsigned xLength, float time, unsigned int& textureAtlas, unsigned int& currentFrame) {
	
	/*m_updateTime = time;
	m_spriteSheet = new Spritesheet(pictureFile, tileWidth, tileHeight, true, true, yStart, xLength);
	m_textureAtlas = m_spriteSheet->getAtlas();
	m_frameCount = m_spriteSheet->getTotalFrames();

	i_textureAtlas = &textureAtlas;
	i_currentFrame = &currentFrame;

	*i_textureAtlas = m_textureAtlas;*/
}

Animation2D::~Animation2D() {}

void Animation2D::create(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, unsigned yStart, unsigned xLength, float time, unsigned int& textureAtlas, unsigned int& currentFrame) {
	
}

void Animation2D::create(Spritesheet* spriteSheet, float time, unsigned int& textureAtlas, unsigned int& currentFrame) {
	
	m_updateTime = time;
	m_spriteSheet = spriteSheet;
	m_frameCount = spriteSheet->getTotalFrames();
	m_textureAtlas = spriteSheet->getAtlas();

	i_textureAtlas = &textureAtlas;
	i_currentFrame = &currentFrame;

	*i_textureAtlas = m_textureAtlas;
}

void Animation2D::update(const float deltaTime) {
	m_elapsedTime += deltaTime;
	while (m_elapsedTime >= m_updateTime) {
		m_elapsedTime -= m_updateTime;
		updateAnimation();
	}
}

void Animation2D::updateAnimation() {
	if (++m_currentFrame > m_frameCount - 1)
		m_currentFrame = 0;
	*i_textureAtlas = m_textureAtlas;
	*i_currentFrame = m_currentFrame;
}

unsigned int Animation2D::getCurrentFrame() const {
	return m_currentFrame;
}

unsigned int Animation2D::getFrameCount() const {
	return m_frameCount;
}

void Animation2D::setCurrentFrame(const unsigned int& frame) {
	m_currentFrame = frame;
	*i_textureAtlas = m_textureAtlas;
	*i_currentFrame = m_currentFrame;
}

void Animation2D::setUpdateTime(const float& time) {
	m_updateTime = time;
}

void Animation2D::setTextureAtlas(const unsigned int& atlas) {
	m_textureAtlas = atlas;
}

unsigned int Animation2D::getTextureAtlas() const {
	return m_textureAtlas;
}

