#include "Animator2D.h"
#include <iostream>

Animator2D::Animator2D(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, unsigned yStart, unsigned xLength, float time, unsigned int& textureAtlas, unsigned int& currentFrame) {
	
	/*m_updateTime = time;
	m_spriteSheet = new Spritesheet(pictureFile, tileWidth, tileHeight, true, true, yStart, xLength);
	m_textureAtlas = m_spriteSheet->getAtlas();
	m_frameCount = m_spriteSheet->getTotalFrames();

	i_textureAtlas = &textureAtlas;
	i_currentFrame = &currentFrame;

	*i_textureAtlas = m_textureAtlas;*/
}

Animator2D::~Animator2D() {}

void Animator2D::create(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, unsigned yStart, unsigned xLength, float time, unsigned int& textureAtlas, unsigned int& currentFrame) {
	/*if (m_frameCount > 0)
		return;

	m_updateTime = time;
	m_spriteSheet = new Spritesheet(pictureFile, tileWidth, tileHeight, true, true, yStart, xLength);
	m_textureAtlas = m_spriteSheet->getAtlas();
	m_frameCount = m_spriteSheet->getTotalFrames();

	i_textureAtlas = &textureAtlas;
	i_currentFrame = &currentFrame;

	*i_textureAtlas = m_textureAtlas;*/
}

void Animator2D::create(Spritesheet* spriteSheet, float time, unsigned int& textureAtlas, unsigned int& currentFrame) {
	
	m_updateTime = time;
	m_spriteSheet = spriteSheet;
	m_frameCount = spriteSheet->getTotalFrames();
	m_textureAtlas = spriteSheet->getAtlas();

	i_textureAtlas = &textureAtlas;
	i_currentFrame = &currentFrame;

	*i_textureAtlas = m_textureAtlas;
}

void Animator2D::update(const float deltaTime) {
	m_elapsedTime += deltaTime;
	while (m_elapsedTime >= m_updateTime) {
		m_elapsedTime -= m_updateTime;
		updateAnimation();
	}
}

void Animator2D::updateAnimation() {
	if (++m_currentFrame > m_frameCount - 1)
		m_currentFrame = 0;
	*i_textureAtlas = m_textureAtlas;
	*i_currentFrame = m_currentFrame;
}

unsigned int Animator2D::getCurrentFrame() const {
	return m_currentFrame;
}

unsigned int Animator2D::getFrameCount() const {
	return m_frameCount;
}

void Animator2D::setCurrentFrame(const unsigned int& frame) {
	m_currentFrame = frame;
	*i_textureAtlas = m_textureAtlas;
	*i_currentFrame = m_currentFrame;
}

void Animator2D::setUpdateTime(const float& time) {
	m_updateTime = time;
}

void Animator2D::setTextureAtlas(const unsigned int& atlas) {
	m_textureAtlas = atlas;
}

unsigned int Animator2D::getTextureAtlas() const {
	return m_textureAtlas;
}

