#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <States/StateMachine.h>

#include "Zone.h"

class Game : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Game(StateMachine& machine);
	~Game();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnReEnter(unsigned int prevState) override;

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void OnKeyUp(Event::KeyboardEvent& event) override;

private:

	void renderUi();

	bool m_initUi = true;
	bool m_drawUi = false;
	bool m_drawCenter = false;
	bool m_useCulling = true;
	bool m_drawScreenBorder = false;
	bool m_debugCollision = false;
	Camera m_camera;
	Zone m_zone;

	float m_movingSpeed;
	float m_viewWidth;
	float m_viewHeight;
	float m_mapHeight;
	float m_screeBorder;
};