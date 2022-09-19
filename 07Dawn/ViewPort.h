#pragma once
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "engine/Camera.h"

class ViewPort {
public:
	
	static ViewPort& get();
	void init(unsigned int width, unsigned int height);
	void update(float dt);
	Camera& getCamera();
	Vector2f& getCursorPos();

	float getLeft();
	float getRight();
	float getBottom();
	float getTop();
	float getWidth();
	float getHeight();

private:
	ViewPort();
	~ViewPort() = default;

	Camera m_camera;
	static ViewPort s_instance;
	float m_left, m_right, m_bottom, m_top;
	const Vector3f& m_postition;
	float m_screeBorder = 100.0f;

	unsigned int m_width;
	unsigned int m_height;

	Vector2f m_cursorPosEye;
};