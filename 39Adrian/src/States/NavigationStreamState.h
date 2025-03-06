#pragma once

#include <vector>
#include <array>
#include <unordered_set>
#include <unordered_map>

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
#include <Navigation/OffMeshConnection.h>
#include <Entities/EmptyAgentEntity.h>
#include <Entities/Woman.h>
#include <Entities/Beta.h>
#include <Entities/Jack.h>

#include <States/StateMachine.h>


enum PhysicalObjects : int {
	MUSHRROM,
	ENTITY,
	GROUND
};

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
	void createBoxOffMeshConnections(DynamicNavigationMesh* navMesh, SceneNodeLC* boxGroup);

	void spawnAgent(const Vector3f& pos);
	void spawnBeta(const Vector3f& pos);
	void spawnJack(const Vector3f& pos);
	void spawnWoman(const Vector3f& pos);
	void createMushroom(const Vector3f& pos);
	void addOrRemoveObject(const Vector3f& pos, PhysicalObjects physicalObjects, Obstacle* obstacle, btCollisionObject* collisionObject);
	void toggleStreaming(bool enabled);
	void updateStreaming();
	void saveNavigationData();

	bool m_initUi = true;
	bool m_drawUi = false;
	bool m_debugTree = false;
	bool m_debugPhysic = false;
	bool m_debugNavmesh = true;
	bool m_useStreaming = false;

	float m_offsetDistance;
	float m_separaionWeight;
	float m_height;
	int m_streamingDistance;

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
	//std::hash_set<std::array<int, 2>> m_addedTiles;

	std::vector<CrowdAgentEntity*> m_entities;
	std::vector<EmptyAgentEntity*> m_empty;

	void addMarker(const Vector3f& pos);

	static std::vector<ShapeNode*> Marker;
	static Octree* _Octree;
	static SceneNodeLC* Root;

	
	std::unordered_set< std::array<int, 2>, std::function<size_t(const std::array<int, 2>&)>, std::function<bool(const std::array<int, 2>&, const std::array<int, 2>&)>> m_addedTiles;
	std::unordered_map< std::array<int, 2>, Buffer, std::function<size_t(const std::array<int, 2>&)>, std::function<bool(const std::array<int, 2>&, const std::array<int, 2>&)>> m_tileData;
};