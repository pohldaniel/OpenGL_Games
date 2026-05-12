#pragma once
#include <functional>

class Fade {

public:

	Fade(float& fadeValue);

	void update(const float dt);
	void toggleFade(bool activate = true);
	void fadeIn(bool activate = true);
	void fadeOut(bool activate = true);
	void start(bool activate = true);
	void setOnFadeEnd(std::function<void()> fun);
	void setOnFadeIn(std::function<void()> fun);
	void setOnFadeOut(std::function<void()> fun);
	void setTransitionSpeed(const float transitionSpeed);
	void setTransitionEnd(bool transitionEnd);
	void setFadeValue(float fadeValue);
	const bool isActivated() const;
	const float getTransitionSpeed() const;

private:

	float& m_fadeValue;
	float m_transitionSpeed;
	bool m_transitionEnd;
	bool m_fadeIn;
	bool m_fadeOut;
	bool m_loop;
	bool m_activate, m_activateIn, m_activateOut;

	std::function<void()> OnFadeEnd;
	std::function<void()> OnFadeIn;
	std::function<void()> OnFadeOut;
};
