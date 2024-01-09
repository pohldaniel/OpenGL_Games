#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/AssimpModel.h>

#include <States/StateMachine.h>
#include "Background.h"

class WireOverlay : public State, public MouseEventListener, public KeyboardEventListener {

public:

	WireOverlay(StateMachine& machine);
	~WireOverlay();

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
	bool m_drawOverlay = true;

	Camera m_camera;
	Background m_background;
	AssimpModel m_model;
	Matrix4f GetViewportMatrix() const;
};