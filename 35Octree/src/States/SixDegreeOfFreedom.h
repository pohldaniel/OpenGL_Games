#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/octree/Octree.h>
#include <engine/scene/ShapeNode.h>
#include <engine/Camera.h>
#include <engine/Frustum.h>
#include <engine/Background.h>

#include <States/StateMachine.h>

class SixDegreeOfFreedom : public State, public MouseEventListener, public KeyboardEventListener {

public:

	SixDegreeOfFreedom(StateMachine& machine);
	~SixDegreeOfFreedom();

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
	void createShapes();
	void createScene();

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_debugTree = false;
	float m_offsetDistance = 10.0f;
	float m_rotationSpeed = 0.1f;

	Camera m_camera;
	Frustum m_frustum;
	Background m_background;

	Shape m_boxShape, m_cylinderShape, m_shipShape, m_hoverbikeShape;

	Octree* m_octree;
	SceneNodeLC* m_root;
};