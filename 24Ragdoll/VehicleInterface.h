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
	btRigidBody* localCreateRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape);
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
	btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;


	void clientResetScene();

	btRigidBody* m_carChassis;
	btTriangleIndexVertexArray* m_indexVertexArrays;
	btVector3*	m_vertices;

	btRaycastVehicle::btVehicleTuning	m_tuning;
	btVehicleRaycaster*	m_vehicleRayCaster;
	btRaycastVehicle*	m_vehicle;
	btCollisionShape*	m_wheelShape;

	float	gEngineForce = 0.f;
	float	gBreakingForce = 0.f;

	float	maxEngineForce = 1000.f;//this should be engine/velocity dependent
	float	maxBreakingForce = 100.f;

	float	gVehicleSteering = 0.0f;
	float	steeringIncrement = 0.04f;
	float	steeringClamp = 0.3f;
	float	wheelRadius = 0.5f;
	float	wheelWidth = 0.4f;
	float	wheelFriction = 1000;//BT_LARGE_FLOAT;
	float	suspensionStiffness = 20.f;
	float	suspensionDamping = 2.3f;
	float	suspensionCompression = 4.4f;
	float	rollInfluence = 0.1f;//1.0f;


	btScalar suspensionRestLength = 0.6f;

	int rightIndex = 0;
	int upIndex = 1;
	int forwardIndex = 2;
	btVector3 wheelDirectionCS0 = btVector3(0.0f, -1.0f, 0.0f);
	btVector3 wheelAxleCS = btVector3(-1.0f, 0.0f, 0.0f);

};
