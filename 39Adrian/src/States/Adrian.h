#pragma once
#include <array>
#include <functional>
#include <unordered_set>
#include <unordered_map>

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/MeshObject/Shape.h>
#include <engine/scene/ShapeNode.h>
#include <engine/scene/Md2Node.h>
#include <engine/octree/Octree.h>
#include <engine/CharacterSet.h>
#include <engine/Framebuffer.h>
#include <engine/Background.h>
#include <engine/Md2Model.h>
#include <engine/Frustum.h>
#include <engine/Camera.h>
#include <engine/Sprite.h>
#include <Navigation/DynamicNavigationMesh.h>
#include <Navigation/Navigable.h>
#include <Navigation/NavArea.h>
#include <Navigation/NavPolygon.h>
#include <Navigation/Obstacle.h>
#include <Navigation/CrowdManager.h>
#include <Navigation/CrowdAgent.h>
#include <Physics/Physics.h>
#include <Physics/MousePicker.h>
#include <States/StateMachine.h>
#include <Entities/Md2Entity.h>
#include <Entities/CrowdAgentEntity.h>
#include <Entities/Hero.h>
#include <Entities/Bot.h>
#include <Utils/Fade.h>

struct EditPolygon {
	int userPointerOffset = 0;
	int size = 0;
	std::vector<Vector3f> edgePoints;
};

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
	void loadBots(const char* filename);
	void createScene(bool recreate = false);
	void loadFont();
	void setCurrentPanelTex(int currentPanelTex);

private:

	void renderUi();
	void toggleStreaming(bool enabled);
	void saveNavigationData();
	void updateStreaming();
	void rebuild();
	void renderScene();
	void renderSceneDepth();
	void renderBubble();
	void spawnHero(const Vector3f& pos);
	void spawnAgent(const Vector3f& pos);

	bool m_initUi = true;
	bool m_drawUi = false;
	bool m_useCulling = true;
	bool m_debugTree = false;
	bool m_debugPhysic = false;
	bool m_debugNavmesh = false;
	bool m_useStreaming = false;
	bool m_drawPolygon = false;

	float m_tileFactor = 80.0f;
	float m_angle = -M_PI_4;
	float m_height = 30.0f;
	float m_zoom = 1.0f;
	int m_streamingDistance;
	float m_markerSize = 20.0f;
	float m_rimScale = 1.0f;
	float m_fadeValue = 0.0f;
	float m_fadeCircleValue = 1.0f;
	float m_separaionWeight;

	IsometricCamera m_camera;

	Md2Model m_heroModel, m_hueteotl, m_mutantman, m_corpse, m_mutantlizard, m_mutantcheetah, m_ripper;

	Hero* m_hero;
	Bot *m_bot1, *m_bot2, *m_bot3, *m_bot4, *m_bot5, *m_bot6, *m_bot7;

	Md2Node *m_md2Node;

	SceneNodeLC* m_root;
	Octree* m_octree;
	Frustum m_frustum;

	Shape m_segment, m_disk, m_sphere;
	ShapeNode *m_segmentNode, *m_diskNode, *m_buildingNode, *m_cursorNode = nullptr;

	MousePicker m_mousePicker;
	btCollisionObject* m_ground;
	std::vector<Shape> m_buildings;

	NavigationMesh* m_navigationMesh;
	CrowdManager* m_crowdManager;
	CrowdAgent* m_agent;

	std::unordered_set< std::array<int, 2>, std::function<size_t(const std::array<int, 2>&)>, std::function<bool(const std::array<int, 2>&, const std::array<int, 2>&)>> m_addedTiles;
	std::vector<EditPolygon> m_editPolygons;
	EditPolygon* m_currentPolygon;

	std::vector<Vector3f> m_edgePoints;
	std::vector<btCollisionObject*> m_collisionObjects;

	int m_globalUserIndex;
	Framebuffer m_depthBuffer;
	Fade m_fade, m_fadeCircle;

	std::vector<Md2Entity*> m_entities;
	Sprite m_panel;
	std::vector<TextureRect> m_tileSet;
	unsigned int m_atlas;
	int m_currentPanelTex;
	CharacterSet set;
};