#pragma once
#include <array>
#include <functional>
#include <unordered_set>
#include <unordered_map>

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/MeshObject/Shape.h>
#include <engine/scene/ShapeNode.h>
#include <engine/octree/Octree.h>
#include <engine/Camera.h>
#include <engine/Background.h>
#include <Navigation/DynamicNavigationMesh.h>
#include <Navigation/Navigable.h>
#include <Navigation/NavArea.h>
#include <Navigation/NavPolygon.h>
#include <Navigation/Obstacle.h>
#include <engine/Frustum.h>
#include <Physics/Physics.h>
#include <Physics/MousePicker.h>
#include <States/StateMachine.h>
#include <Entities/Md2Entity.h>

class Adrian : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Adrian(StateMachine& machine);
	~Adrian();

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

	void loadBuilding(const char* filename, bool changeWinding = false);
	void createScene(bool recreate = false);

private:

	void renderUi();
	void clearMarker();
	void toggleStreaming(bool enabled);
	void saveNavigationData();
	void updateStreaming();
	void rebuild();

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_useCulling = true;
	bool m_debugTree = false;
	bool m_debugPhysic = true;
	bool m_debugNavmesh = true;
	bool m_useStreaming = false;
	bool m_drawPolygon = true;

	float m_tileFactor = 80.0f;
	float m_angle = -M_PI_4;
	float m_height = 30.0f;
	float m_zoom = 1.0f;
	int m_streamingDistance;
	float m_markerSize = 20.0f;

	IsometricCamera m_camera;

	Md2Model m_hero;
	Md2Entity *m_heroEnity;

	SceneNodeLC* m_root;
	Octree* m_octree;
	Frustum m_frustum;

	Shape m_segment, m_disk, m_sphere;
	ShapeNode *m_segmentNode, *m_diskNode, *m_buildingNode;

	std::vector<ShapeNode*> m_marker;

	MousePicker m_mousePicker;
	btCollisionObject* m_ground;
	std::vector<Shape> m_buildings;

	NavigationMesh* m_navigationMesh;

	std::unordered_set< std::array<int, 2>, std::function<size_t(const std::array<int, 2>&)>, std::function<bool(const std::array<int, 2>&, const std::array<int, 2>&)>> m_addedTiles;
	std::vector<Vector3f> m_edgePoints;
	std::vector<btCollisionObject*> m_collisionObjects;

	int m_globalUserIndex;
};