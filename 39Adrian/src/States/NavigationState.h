#pragma once

#include <engine/input/KeyboardEventListener.h>
#include <engine/input/MouseEventListener.h>
#include <engine/animationNew/AnimationController.h>
#include <engine/animationNew/AnimatedModel.h>
#include <engine/MeshObject/Shape.h>
#include <engine/Scene/ShapeNode.h>
#include <engine/octree/Octree.h>
#include <engine/Background.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>

class NavigationState : public State, public MouseEventListener, public KeyboardEventListener {

public:

	NavigationState(StateMachine& machine);
	~NavigationState();

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

	Camera m_camera;
	Frustum m_frustum;
	Background m_background;

	SceneNodeLC* m_root;
	Octree* m_octree;

	Shape m_ground, m_cylinder, m_cube, m_cube14, m_cube17;
	AnimatedModel m_beta;
};