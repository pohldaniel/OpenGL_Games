#include "Animator.h"

Animator::Animator(unsigned framesX, unsigned framesY, float time, unsigned yStart) {
	
	unsigned frameCount = (framesX + 1) * (framesY > 0 ? framesY : 1);
	m_updateTime = time;
	m_frameCount = frameCount;
	m_spriteSheet = new Spritesheet("res/textures/player.png", 96, 84, true, true, yStart, framesX);
}

Animator::~Animator() {}

void Animator::create(unsigned framesX, unsigned framesY, float time, unsigned yStart) {
	if (m_frameCount > 0)
		return;

	unsigned frameCount = (framesX + 1) * (framesY > 0 ? framesY : 1);
	m_updateTime = time;
	m_frameCount = frameCount;
	m_spriteSheet = new Spritesheet("res/textures/player.png", 96, 84, true, true, yStart, framesX);
	m_textureAtlas = m_spriteSheet->getAtlas();
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
}

unsigned Animator::getCurrentFrame() const {
	return m_currentFrame;
}

unsigned Animator::getFrameCount() const {
	return m_frameCount;
}

void Animator::setFrame(const unsigned& frame) {
	m_currentFrame = frame;
}

unsigned int Animator::getAtlas() {
	return m_textureAtlas;
}