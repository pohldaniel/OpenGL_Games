#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Framebuffer.h>
#include <engine/Camera.h>
#include <engine/Background.h>
#include <engine/AssimpModel.h>
#include <States/StateMachine.h>

class SSSApproximation : public State, public MouseEventListener, public KeyboardEventListener {

public:

	SSSApproximation(StateMachine& machine);
	~SSSApproximation();

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
	bool m_useDiffuse = false;

	Camera m_camera;
	Background m_background;
	AssimpModel m_model;

	Vector3f m_lightPos;
	Matrix4f m_lightView, m_lightViewInverse;
	Matrix4f m_lightProjection;
	Matrix4f m_lightViewProjection;
	Matrix4f m_lightTexcoord;

	Framebuffer m_irradiance, m_distance;
	const float maxDistance[1] = { 0.0f };
};