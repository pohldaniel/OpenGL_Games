#pragma once

#include <functional>

class Timer {
public:
	void update(const float& dt);
	void setFunction(float time, std::function<void()> fun);
	void setUpdateTime(float time);
	float getUpdateTime() const;

private:
	float m_elapsedTime = 0.0f;
	float m_updateTime = 9999999.0f;
	std::function<void()> m_fun;
};