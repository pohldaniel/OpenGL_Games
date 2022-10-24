#pragma once
#include <vector>
#include "../Rect.h"

class Animation2D {

public:
	
	Animation2D() = default;
	~Animation2D();

	void update(const float deltaTime);
	void start();
	void addFrame(const TextureRect& frame);
	const Animation2D::TextureRect& getFrame();
	const bool waitForAnimation() const;
	void resetAnimation();
	unsigned int getNumberOfFrames();

private:

	unsigned int m_frameCount = 0;
	float m_elapsedTime = 0.0f;
	unsigned int m_currentFrame = 0;
	TextureRect m_frame;
	std::vector<TextureRect> m_frames;

	bool m_waitForAnimation = false;
	bool m_loop = true;
};
