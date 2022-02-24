#pragma once
#include "Spritesheet.h"

class Animator {
public:
	Animator() = default;
	Animator(unsigned framesX, unsigned framesY, float time, unsigned YStart = 0);
	~Animator();

	void create(unsigned framesX, unsigned framesY, float time, unsigned YStart = 0);
	void update(const float deltaTime);

	// Accessors
	unsigned getCurrentFrame() const;
	unsigned getFrameCount() const;
	void setFrame(const unsigned& frame);
	unsigned int getAtlas();

	
private:
	Spritesheet *m_spriteSheet;

	unsigned m_currentFrame = 0;
	unsigned m_frameCount = 0;
	unsigned int m_textureAtlas;

	float m_elapsedTime = 0.f;
	float m_updateTime = 0.f;

	void updateAnimation();
};
