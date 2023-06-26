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

#define BT_ACTIVE_SHIFT	0x0001
#define MAX_TRIANGLES_IN_BATCH 8192


class Game : public State, public MouseEventListener, public KeyboardEventListener {

public:

	struct ShapeCache
	{
		struct Edge { btVector3 n[2]; int v[2]; };
		ShapeCache(btConvexShape* s) : m_shapehull(s) {}
		btShapeHull					m_shapehull;
		btAlignedObjectArray<Edge>	m_edges;
	};

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

	btDynamicsWorld* m_dynamicsWorld;
	btTypedConstraint* m_pickConstraint;

	float	m_ShootBoxInitialSpeed;
	int	m_modifierKeys;
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
