#pragma once

#include <engine/Camera.h>

class ViewPort {

public:

	void resize();
	Camera& getCamera();
	void move(const Vector3f& delta);

	const Vector3f& getPosition();
	float getLeft();
	float getRight();
	float getBottom();
	float getTop();

	void setPosition(const Vector3f& position);
	void setPosition(float x, float y, float z);
	void setScreenBorder(float screeBorder);

	static ViewPort& Get();
	static bool IsRectOnScreen(float posX, float posY, float width, float height);

private:

	ViewPort();
	~ViewPort() = default;

	Camera m_camera;
	float m_left, m_right, m_bottom, m_top;
	const Vector3f& postition;
	float m_screeBorder;

	static ViewPort s_instance;
	static float ViewLeft, ViewRight, ViewBottom, ViewTop;
	static bool Update;
};