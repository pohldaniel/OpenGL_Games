#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/MeshObject/Shape.h>
#include <engine/Camera.h>
#include <engine/Framebuffer.h>
#include <States/StateMachine.h>

class Shadow : public State, public MouseEventListener, public KeyboardEventListener {

	static const int DEPTH_TEXTURE_SIZE = 1024;

public:

	Shadow(StateMachine& machine);
	~Shadow();

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

	Camera m_camera;
	Matrix4f lightProjection;
	Matrix4f lightView;
	Matrix4f model;
	Vector3f lightPosition;

	Framebuffer m_depthRT;
	Shape m_armadillo;
};