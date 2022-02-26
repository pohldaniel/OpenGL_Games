#pragma once
#include "Spritesheet.h"

class Animator {
public:
	Animator() = default;
	Animator(std::string pictureFile, unsigned YStart, unsigned xLength, float time, unsigned int& textureAtlas, unsigned int& currentFrame);
	~Animator();

	void create(std::string pictureFile, unsigned YStart, unsigned xLength, float time, unsigned int& textureAtlas, unsigned int& currentFrame);
	void update(const float deltaTime);

	// Accessors
	unsigned int getCurrentFrame() const;
	unsigned int getFrameCount() const;
	unsigned int getTextureAtlas() const;
	void setCurrentFrame(const unsigned int& frame);
	void setTextureAtlas(const unsigned int& atlas);

private:

	Spritesheet *m_spriteSheet;
	
	unsigned m_frameCount = 0;
	float m_elapsedTime = 0.f;
	float m_updateTime = 0.f;

	unsigned int m_textureAtlas;
	unsigned int m_currentFrame = 0;

	unsigned int* i_textureAtlas;
	unsigned int* i_currentFrame;

	void updateAnimation();
};
