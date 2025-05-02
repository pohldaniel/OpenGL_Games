#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/MeshObject/Shape.h>
#include <engine/Camera.h>
#include <engine/Background.h>
#include <engine/Framebuffer.h>
#include <States/StateMachine.h>

class Winston : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Winston(StateMachine& machine);
	~Winston();

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
	void renderBubble();
	void renderSceneDepth();
	void renderScene();

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;
	Shape m_world, m_sphere;
	Vector3f m_bubblePos;
	Framebuffer m_depthBuffer;
};