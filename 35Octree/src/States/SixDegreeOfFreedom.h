#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/octree/Octree.h>
#include <engine/Camera.h>
#include <engine/Frustum.h>
#include <engine/Background.h>

#include <States/StateMachine.h>
#include <Physics/Physics.h>
#include <Entities/ShapeEntity.h>
#include "SplinePath.h"

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
	void createPhysics();
	void createScene();
	void updateSplinePath(float timeStep);

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_debugTree = false;
	bool m_useCulling = true;
	bool m_useOcclusion = true;
	bool m_debugPhysic = true;
	bool m_drawDebug = false;

	float m_offsetDistance = 10.0f;
	float m_rotationSpeed = 0.1f;

	Camera m_camera;
	Frustum m_frustum;
	Shape m_boxShape, m_cylinderShape, m_shipShape, m_hoverbikeShape;
	Octree* m_octree;
	SplinePath* m_splinePath;
	SceneNodeLC* m_root;

	btCollisionObject* m_kinematicBox;
};