#pragma once

#include <engine/input/Event.h>
#include <engine/Camera.h>

class CameraController {

public:

	CameraController(IsometricCamera& camera, const int& width, const int& height);

	void update(float dt);
	void setSpeed(float val);

	void OnMouseMotion(Event::MouseMoveEvent& event);
	void OnKeyDown(Event::KeyboardEvent& event);
	void OnKeyUp(Event::KeyboardEvent& event);

private:

	void OnMouseMotion();

	IsometricCamera& camera;
	const int& width;
	const int& height;

	float m_speed;

	int m_dirX;
	int m_dirY;

	bool m_keyScrollX = false;
	bool m_keyScrollY = false;
	bool m_mouseScrollX = false;
	bool m_mouseScrollY = false;

	bool m_mouseWasScrollX = false;
	bool m_mouseWasScrollY = false;

	int m_screenX = 0;
	int m_screenY = 0;
};