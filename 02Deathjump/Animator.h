#pragma once

class Animator {
public:
	Animator() = default;

	Animator(unsigned framesX, unsigned framesY, float time, unsigned YStart = 0);

	void Create(unsigned framesX, unsigned framesY, float time, unsigned YStart = 0);
	void Update(const float deltaTime);

	// Accessors
	unsigned GetCurrentFrame() const;
	unsigned GetFrameCount() const;
	void SetFrame(const unsigned& frame);
	void SetUpdateTime(const float& time);

	~Animator();
private:
	unsigned m_currentFrame = 0;
	unsigned m_frameCount = 0;

	//sf::IntRect* m_frames = nullptr;
	//sf::Sprite* m_sprite = nullptr;

	float m_elapsedTime = 0.f;
	float m_updateTime = 0.f;

	void UpdateAnimation();
	void CutFrames(const unsigned& framesX, const unsigned& framesY, const unsigned& YStart);
};
