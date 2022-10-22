#include "Animation2D.h"
#include <iostream>

Animation2D::~Animation2D() {}


void Animation2D::update(const float deltaTime) {
	
	if (m_waitForAnimation) {
		
		m_elapsedTime = m_elapsedTime >= m_frameCount ? 0.0f : m_elapsedTime + deltaTime * 12;
		m_currentFrame = static_cast<unsigned short>(floor(m_elapsedTime));
		if (++m_currentFrame > m_frameCount - 1) {
			m_currentFrame = 0;
			m_waitForAnimation = m_loop;

		}	
		m_frame = m_frames[m_currentFrame];
	}
}

void Animation2D::addFrame(const TextureRect& frame) {
	m_frames.push_back(frame);
	m_frameCount = static_cast<unsigned int>(m_frames.size());
	m_frame = m_frames[0];
}

void Animation2D::resetAnimation() {
	m_frame = m_frames[0];
}

const Animation2D::TextureRect& Animation2D::getFrame() {
	return m_frame;
}

void Animation2D::start() {
	m_waitForAnimation = true;
	m_elapsedTime = 0.0f;
}

const bool Animation2D::waitForAnimation() const{
	return m_waitForAnimation;
}

unsigned int Animation2D::getNumberOfFrames() {
	return m_frameCount;
}