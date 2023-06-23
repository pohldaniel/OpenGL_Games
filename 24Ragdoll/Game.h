#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"
#include "engine/Camera.h"
#include "engine/TrackBall.h"
#include "StateMachine.h"
#include "Physics.h"
#include "Ragdoll.h"
#include "GL_ShapeDrawer.h"
#include "MousePicker.h"

#define BT_ACTIVE_SHIFT	0x0001


class Game : public State, public MouseEventListener, public KeyboardEventListener {

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
	void renderUi();

	btAlignedObjectArray<class RagDoll*> m_ragdolls;
	btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;
	void spawnRagdoll(const btVector3& startOffset);

	
	virtual void removePickingConstraint();
	virtual void pickObject(const btVector3& pickPos, const class btCollisionObject* hitObj);
	void renderme();

	btDynamicsWorld* m_dynamicsWorld;

	///constraint for mouse picking
	btTypedConstraint* m_pickConstraint;

	float	m_ShootBoxInitialSpeed;

	bool	m_stepping;
	bool m_singleStep;
	bool m_idle;
	int m_lastKey;
	int	m_modifierKeys;

	void renderscene(int pass);

	GL_ShapeDrawer*	m_shapeDrawer;
	int gPickingConstraintId = 0;
	btVector3 gOldPickingPos;
	btVector3 gHitPos = btVector3(-1, -1, -1);
	btScalar gOldPickingDist = 0.f;
	btRigidBody* pickedBody = 0;//for deactivation state
	btScalar mousePickClamping = 30.f;

private:

	Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;
	MousePicker m_mousePicker;

	bool m_initUi = true;
	bool m_drawUi = true;


};
