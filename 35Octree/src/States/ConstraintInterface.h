#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>
#include <Physics/Physics.h>
#include <Physics/MousePicker.h>



class ConstraintInterface : public State, public MouseEventListener, public KeyboardEventListener {

public:

	ConstraintInterface(StateMachine& machine);
	~ConstraintInterface();

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
	void removePickingConstraint();
	void pickObject(const btVector3& pickPos, const class btCollisionObject* hitObj);

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_debugPhysic = true;
	bool m_testConeTwistMotor = false;
	float m_offsetDistance = 0.0f;
	float m_rotationSpeed = 0.1f;
	float m_time = 0.0f;

	Camera m_camera;
	MousePicker m_mousePicker;
	btRigidBody* localCreateRigidBodyPick(float mass, const btTransform& startTransform, btCollisionShape* shape);
	btRigidBody* localCreateRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape);
	btScalar m_defaultContactProcessingThreshold;
	btConeTwistConstraint* m_ctc;

	btTypedConstraint* m_pickConstraint = nullptr;
	btRigidBody* pickedBody = 0;
	btScalar mousePickClamping = 30.f;
};
