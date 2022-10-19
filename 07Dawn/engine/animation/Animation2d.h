#pragma once
#include <vector>

class Animation2D {

public:
	struct TextureRect {
		float textureOffsetX;
		float textureOffsetY;
		float textureWidth;
		float textureHeight;
		int height;
		int width;
		unsigned int frame;
	};


	Animation2D() = default;
	~Animation2D();

	void update(const float deltaTime);
	void start();
	void addFrame(const TextureRect& frame);
	const Animation2D::TextureRect& getFrame();
	const bool waitForAnimation() const;

private:

	unsigned m_frameCount = 0;
	float m_elapsedTime = 0.f;
	unsigned int m_currentFrame = 0;
	TextureRect m_frame;
	std::vector<TextureRect> m_frames;

	bool m_waitForAnimation = false;
	bool m_loop = false;
};
