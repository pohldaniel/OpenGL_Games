#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/Background.h>
#include <engine//MeshObject/MeshDisk.h>
#include <States/StateMachine.h>

class Controller : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Controller(StateMachine& machine);
	~Controller();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(const Event::MouseMoveEvent& event) override;
	void OnMouseWheel(const Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(const Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(const Event::MouseButtonEvent& event) override;
	void OnKeyDown(const Event::KeyboardEvent& event) override;
	void OnKeyUp(const Event::KeyboardEvent& event) override;

private:

	void renderUi();
	void renderPad(float x, float y, long value);
	void renderButton(float x, float y, long value, int index);

	bool m_initUi = true;
	bool m_drawUi = false;
	long m_state = 0;
	Camera m_camera;
	MeshDisk m_meshDisk;
};