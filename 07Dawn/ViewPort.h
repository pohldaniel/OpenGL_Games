#pragma once
#include "engine/input/KeyBorad.h"
#include "engine/Camera.h"

class ViewPort {
public:
	
	static ViewPort& get();
	void init();
	void update(float dt);
	Camera& getCamera();

	float getLeft();
	float getRight();
	float getBottom();
	float getTop();

private:
	ViewPort();
	~ViewPort() = default;

	Camera m_camera;
	static ViewPort s_instance;
	float m_left, m_right, m_bottom, m_top;
	const Vector3f& m_postition;
	float m_screeBorder = 100.0f;
};