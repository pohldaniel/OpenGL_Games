#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Framebuffer.h>
#include <engine/Camera.h>
#include <engine/Background.h>

#include <States/StateMachine.h>

class RayMarch : public State, public MouseEventListener, public KeyboardEventListener {

public:

	RayMarch(StateMachine& machine);
	~RayMarch();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void OnKeyUp(Event::KeyboardEvent& event) override;

private:

	void renderUi();

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_drawOffscreen = false;

	Camera m_camera;
	Background m_background;
	Framebuffer m_sceneBuffer;
	const float maxDistance[1] = { 100000.0f };
};