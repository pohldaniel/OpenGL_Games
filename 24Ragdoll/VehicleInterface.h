#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"
#include "engine/Camera.h"
#include "engine/TrackBall.h"
#include "StateMachine.h"
#include "Physics.h"
#include "Ragdoll.h"
#include "MousePicker.h"
#include "ShapeDrawer.h"
#include "PhysicsCar.h"

#define CUBE_HALF_EXTENTS 1

class VehicleInterface : public State, public MouseEventListener, public KeyboardEventListener {

public:

	VehicleInterface(StateMachine& machine);
	~VehicleInterface();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;

private:

	void removePickingConstraint();
	void pickObject(const btVector3& pickPos, const class btCollisionObject* hitObj);
	void renderUi();

	Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;
	MousePicker m_mousePicker;

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_follow = true;

	btTypedConstraint* m_pickConstraint;
	btRigidBody* pickedBody = 0;
	btScalar mousePickClamping = 30.0f;
	
	btTriangleIndexVertexArray* m_indexVertexArrays;
	btVector3*	m_vertices;

	PhysicsCar* m_physicsCar;
};
