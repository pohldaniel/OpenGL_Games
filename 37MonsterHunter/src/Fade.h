#pragma once
#include <functional>

class Fade {

public:

	Fade(float& fadeValue);

	void update(const float dt);
	void toggleFade(bool activate = true);
	void fadeIn(bool activate = true);
	void fadeOut(bool activate = true);
	void setOnFadeEnd(std::function<void()> fun);
	void setOnFadeIn(std::function<void()> fun);
	void setOnFadeOut(std::function<void()> fun);
	void setTransitionSpeed(const float transitionSpeed);

private:

	std::function<void()> OnFadeEnd;
	std::function<void()> OnFadeIn;
	std::function<void()> OnFadeOut;
	float m_transitionSpeed;
	bool m_transitionEnd;
	bool m_fadeIn;
	bool m_fadeOut;
	float& m_fadeValue;
	bool m_activate, m_activateIn, m_activateOut;
};
