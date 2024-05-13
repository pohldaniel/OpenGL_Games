#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/octree/Octree.h>
#include <engine/Camera.h>
#include <engine/Frustum.h>
#include <engine/Background.h>

#include <States/StateMachine.h>

#include "CharacterSkinned.h"
#include "SplinePath.h"
#include "SplinePlatform.h"
#include "MovingPlatform.h"
#include "Lift.h"

class SkinnedArmor : public State, public MouseEventListener, public KeyboardEventListener {

public:

	SkinnedArmor(StateMachine& machine);
	~SkinnedArmor();

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

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_useCulling = true;
	bool m_useOcclusion = true;
	bool m_debugTree = false;
	bool m_overview = false;
	bool m_debugPhysic = false;
	float m_fovx = 45.0f;
	float m_far = 110.0f;
	float m_near = 0.1f;
	float m_prevFraction = 1.0f;
	float m_offsetDistance = 10.0f;
	float m_rotationSpeed = 0.1f;

	Camera m_camera;
	Frustum m_frustum;
	Matrix4f perspective, m_view;

	KinematicCharacterController* m_characterController;
	CharacterSkinned* m_characterSkinned;

	SceneNodeLC* m_root;
	Octree* m_octree;
	MovingPlatform* m_movingPlatform;
	SplinePlatform* m_splinePlatform;
	SplinePath* m_splinePath;
	Lift* m_lift;
	std::vector<ShapeNode*> m_entities;

	Shape m_baseShape;
	Shape m_upperFloorShape;
	Shape m_rampShape;
	Shape m_ramp2Shape;
	Shape m_ramp3Shape;
	Shape m_liftShape;
	Shape m_liftShapeExtend;
	Shape m_liftExteriorShape;
	Shape m_liftButtonShape;
	Shape m_diskShape;
	Shape m_cylinderShape;
	Shape m_dummy;

	btCollisionObject* m_kinematicLift;
	btCollisionObject* m_liftTrigger;
	btCollisionObject* m_liftButtonTrigger;
	btCollisionObject* m_kinematicPlatform1;
	btCollisionObject* m_kinematicPlatform2;
	btCollisionObject* m_dummyTrigger;
};