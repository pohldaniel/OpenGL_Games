#pragma once

#include <vector>
#include <engine/input/KeyboardEventListener.h>
#include <engine/input/MouseEventListener.h>
#include <engine/animationNew/AnimationController.h>
#include <engine/animationNew/AnimatedModel.h>
#include <engine/MeshObject/Shape.h>
#include <engine/Scene/ShapeNode.h>
#include <engine/octree/Octree.h>
#include <engine/Camera.h>
#include <Physics/Physics.h>
#include <Physics/MousePicker.h>
#include <Navigation/NavigationMesh.h>
#include <Navigation/Navigable.h>
#include <Navigation/CrowdManager.h>
#include <Navigation/CrowdAgent.h>

#include <States/StateMachine.h>

enum NavFlag
{
	NAVPOLYFLAG_LEVEL0 = 0,
	NAVPOLYFLAG_LEVEL1 = (1 << 0),
	NAVPOLYFLAG_LEVEL2 = (1 << 1),
	NAVPOLYFLAG_LEVEL3 = (1 << 2),
};

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
	void createPhysics();
	void createScene();
	void clearMarker();

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_debugTree = false;
	bool m_debugPhysic = false;
	bool m_debugNavmesh = false;
	float m_offsetDistance;

	Camera m_camera;
	Frustum m_frustum;

	SceneNodeLC* m_root;
	Octree* m_octree;

	Shape m_ground, m_cylinder, m_cube, m_cube14, m_cube17, m_sphere;
	AnimatedModel m_beta, m_jack;
	Animation* m_animationNode;
	std::vector<ShapeNode*> m_marker;

	MousePicker m_mousePicker;
	btCollisionObject* m_groundObject;
	NavigationMesh* m_navigationMesh;
	std::vector<Navigable*> m_navigables;
	CrowdManager* m_crowdManager;
	CrowdAgent* m_crowdAgent;
	AnimationController *m_animationControllerBeta, *m_animationControllerJack;
};