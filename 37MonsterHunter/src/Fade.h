#pragma once
#include <functional>

class Fade {

public:

	Fade(float& fadeValue);

	void update(const float dt);
	void toggleFade();
	void setOnFadeEnd(std::function<void()> fun);
	void setTransitionSpeed(const float transitionSpeed);

private:

	std::function<void()> OnFadeEnd;
	float m_transitionSpeed;
	bool m_transitionEnd;
	bool m_fadeIn;
	bool m_fadeOut;
	float& m_fadeValue;
	bool m_activate;
};
