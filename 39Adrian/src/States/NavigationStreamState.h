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
#include <Navigation/DynamicNavigationMesh.h>
#include <Navigation/Navigable.h>
#include <Navigation/CrowdManager.h>
#include <Navigation/CrowdAgent.h>
#include <Navigation/Obstacle.h>
#include <Entities/EmptyAgentEntity.h>
#include <Entities/Woman.h>
#include <Entities/Beta.h>
#include <Entities/Jack.h>

#include <States/StateMachine.h>


class NavigationStreamState : public State, public MouseEventListener, public KeyboardEventListener {

public:

	NavigationStreamState(StateMachine& machine);
	~NavigationStreamState();

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

	static void AddMarker(const Vector3f& pos);

private:

	void renderUi();
	void createShapes();
	void createPhysics();
	void createScene();
	void clearMarker();

	void spawnAgent(const Vector3f& pos);
	void spawnBeta(const Vector3f& pos);
	void spawnJack(const Vector3f& pos);
	void spawnWoman(const Vector3f& pos);
	void createMushroom(const Vector3f& pos);

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_debugTree = false;
	bool m_debugPhysic = false;
	bool m_debugNavmesh = true;

	float m_offsetDistance;
	float m_separaionWeight;
	float m_height;

	NavigationPushiness m_mode = NavigationPushiness::NAVIGATIONPUSHINESS_MEDIUM;

	Camera m_camera;
	Frustum m_frustum;

	Shape m_ground, m_cylinder, m_cube, m_cube14, m_cube17, m_plane, m_box, m_mushroom;
	AnimatedModel m_beta, m_jack, m_woman;
	Animation* m_animationNode;

	MousePicker m_mousePicker;
	DynamicNavigationMesh* m_navigationMesh;
	std::vector<Navigable*> m_navigables;

	CrowdManager* m_crowdManager;
	CrowdAgent *m_crowdAgentBeta, *m_crowdAgentJack;
	AnimationController *m_animationControllerBeta, *m_animationControllerJack;


	std::vector<CrowdAgentEntity*> m_entities;

	void addMarker(const Vector3f& pos);


	static std::vector<ShapeNode*> Marker;
	static Octree* _Octree;
	static SceneNodeLC* Root;
};