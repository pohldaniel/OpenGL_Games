#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"
#include "engine/Camera.h"
#include "engine/TrackBall.h"
#include "StateMachine.h"
#include "Physics.h"
#include "StaticTrigger.h"

class Game : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Game(StateMachine& machine);
	~Game();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void renderUi();

private:

	Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;

	bool m_initUi = true;
	bool m_drawUi = true;

	StaticTrigger m_ground;
};
