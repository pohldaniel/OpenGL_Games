#pragma once
#include "Vector.h"

class ViewEffect {
public:
	static ViewEffect& get();

	const Matrix4f& getView();

	void update(const float& dt);

	void shake(const float& duration);
	void init();
private:
	ViewEffect() { }
	~ViewEffect() {}

	Matrix4f m_view;

	Vector2f m_positions[15];

	int m_currentPosition = 0;

	const float m_updateTime = 0.02f;
	float m_elapsedTime = 0.0f;

	float m_duration = 0.0f;

	static ViewEffect s_instance;

	void applayEffect();
	
};