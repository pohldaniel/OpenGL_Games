#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"
#include "engine/MeshObject/Shape.h"
#include "engine/Camera.h"
#include "engine/TrackBall.h"
#include "StateMachine.h"
#include "Physics.h"
#include "Ragdoll.h"
#include "MousePicker.h"
#include "ShapeDrawer.h"
#include "SolidIO.h"
#include "Player.h"
#include "LiftButton.h"

#include "lugaru/Graphic/ModelLu.hpp"

struct LiftButtonTriggerCallback : public btCollisionWorld::ContactResultCallback {
	float buttonOffset = 0.0f;
	btScalar LiftButtonTriggerCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override;
};

class Game : public State, public MouseEventListener, public KeyboardEventListener{

public:

	Game(StateMachine& machine);
	~Game();

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

	void applyTransformation(TrackBall& arc);
	void renderUi();
	void removePickingConstraint();
	void pickObject(const btVector3& pickPos, const class btCollisionObject* hitObj);

	Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;
	MousePicker m_mousePicker;

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_debugPhysic = true;

	btTypedConstraint* m_pickConstraint;
	btRigidBody* pickedBody = 0;
	btScalar mousePickClamping = 30.f;

	ModelLu model;
	Utils::SolidIO solidConverter;
	Utils::MdlIO mdlConverter;

	std::vector<float> vertexBuffer;
	std::vector<unsigned int> indexBuffer;
	Shape m_sword;
	Shape m_rabbit;
	Shape m_disk;
	Shape m_lift;
	Shape m_liftButton;
	Shape m_base;
	Shape m_liftExterior;
	Shape m_upperFloor;
	Shape m_ramp;
	Shape m_ramp2;
	Shape m_ramp3;
	Shape m_cylinder;

	Player m_player;
	LiftButton m_pLiftButton;

	LiftButtonTriggerCallback m_liftButtonTriggerResult;

	float buttonPressedHeight_ = 15.0f;
	float buttonOffset = 0.0f;
};
