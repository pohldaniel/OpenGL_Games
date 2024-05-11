#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/MeshObject/Shape.h>
#include <engine/octree/Octree.h>
#include <engine/Scene/ShapeNode.h>
#include <engine/Scene/AnimationNode.h>
#include <engine/Camera.h>
#include <engine/Frustum.h>

#include <Animation/AnimationController.h>
#include <Physics/Physics.h>
#include <Utils/SolidIO.h>
#include <States/StateMachine.h>

#include "MovingPlatform.h"
#include "SplinePlatform.h"
#include "SplinePath.h"
#include "Lift.h"
#include "CharacterController.h"
#include "Character.h"

class KCCInterface : public State, public MouseEventListener, public KeyboardEventListener {

public:

	KCCInterface(StateMachine& machine);
	~KCCInterface();

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

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_useCulling = true;
	bool m_useOcclusion = true;
	bool m_debugTree = false;
	bool m_overview = false;
	bool m_debugPhysic = false;

	Camera m_camera;
	Frustum m_frustum;
	Utils::MdlIO mdlConverter;

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

	btCollisionObject* m_kinematicLift;
	btCollisionObject* m_kinematicLiftTrigger;
	btCollisionObject* m_liftButtonTrigger;
	btCollisionObject* m_kinematicPlatform1;
	btCollisionObject* m_kinematicPlatform2;

	SceneNodeLC* m_root;
	Octree* m_octree;
	std::vector<ShapeNode*> m_entities;

	float m_fovx = 45.0f;
	float m_far = 110.0f;
	float m_near = 0.1f;
	float m_distance = 0.01f;
	float m_prevFraction = 1.0f;
	float m_rotationSpeed = 0.1f;
	float m_offsetDistance = 10.0f;

	Matrix4f perspective, m_view;

	MovingPlatform* m_movingPlatform;
	SplinePlatform* m_splinePlatform;
	SplinePath* m_splinePath;
	Lift* m_lift;

	AnimatedModel m_beta;
	AnimationController* m_animController;
	CharacterController* m_characterController;
	Character* m_character;

	AnimationNode* m_betaNode;
};