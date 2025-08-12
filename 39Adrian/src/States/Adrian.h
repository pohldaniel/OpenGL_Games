#pragma once

#include <array>

#include <engine/input/KeyboardEventListener.h>
#include <engine/input/MouseEventListener.h>
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
#include <engine/Rect.h>
#include <Navigation/DynamicNavigationMesh.h>
#include <Navigation/NavPolygon.h>
#include <Navigation/Navigable.h>
#include <Navigation/NavArea.h>
#include <Navigation/Obstacle.h>
#include <Navigation/CrowdManager.h>
#include <Navigation/CrowdAgent.h>
#include <Physics/MousePicker.h>
#include <Physics/Physics.h>
#include <Entities/CrowdAgentEntity.h>
#include <Entities/Md2Entity.h>
#include <Entities/Hero.h>
#include <Entities/Bot.h>
#include <Utils/NavPolygonHelper.h>
#include <Utils/Fade.h>
#include <States/StateMachine.h>

#include "CameraController.h"
#include "Billboard.h"
#include "MiniMap.h"
#include "Scene.h"

#define		SCR2RESX(x)		((x)/640.0 * (1024))
#define		SCR2RESY(y)		((y)/480.0 * (768))

static const char *arrow[] = {
	/* width height num_colors chars_per_pixel */
	"    32    32        3            1",
	/* colors */
	"+ c #FFFFFF",
	". c #000000",
	"  c None",
	/* pixels */
		"..                              ",
		"....                            ",
		" .....                          ",
		" ..+++...                       ",
		"  .+.++....                     ",
		"  ..+.+++....                   ",
		"  ..++.++++....                 ",
		"   ..++.+++++....               ",
		"   ..+++.++++++....             ",
		"   ..++++.+++++++....           ",
		"    ..++++.+++++++++...         ",
		"    ..++++..+++++++++....       ",
		"    ...++++..++++++++++....     ",
		"     ..+++++..++++++++++.....   ",
		"     ..++++++..+++++++++++....  ",
		"      ..++++++..++++++++++++..  ",
		"      ..+++++++..+++++++++++.   ",
		"      ...+++++++..++++++++++.   ",
		"       ..++++++++..+++++++++.   ",
		"       ..+++++++++..++++++++.   ",
		"       ...++++++++...+++++++.   ",
		"        ..+++++++++...+++++.    ",
		"        ...+++++++++...++++.    ",
		"        ...++++++++++...+++.    ",
		"         ..+++++++++++...++..   ",
		"           ...+++++++++.......  ",
		"               ....++++....  .  ",
		"                   .......      ",
		"                        .....   ",
		"                        .....   ",
		"                         .      ",
		"                                ",
		"0,0"
};

class Adrian : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Adrian(StateMachine& machine, const std::string& background = "data/textures/buildings/ground.tga");
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

private:

	void renderUi();
	void toggleStreaming(bool enabled);
	void saveNavigationData();
	void updateStreaming();
	void rebuild();
	void renderScene();
	void renderSceneDepth();
	void renderSceneShadow();
	void renderBubble();
	void spawnHero(const Vector3f& pos);
	void spawnAgent(const Vector3f& pos);
	void setTarget(const Vector3f& pos);
	void createCollisionFilter();
	void activateHero();
	void centerHero();
	void createScene(bool recreate = false);
	void initBots();

	bool m_initUi = true;
	bool m_drawUi = false;
	bool m_useCulling = true;
	bool m_debugTree = false;
	bool m_debugPhysic = false;
	bool m_debugNavmesh = false;
	bool m_useStreaming = false;

	bool m_invisible = false;
	bool m_showHelp = false;
	bool m_showPanel = true;
	bool m_noWalls = false;
	bool m_roatecamera = false;

	float m_tileFactor = 80.0f;
	float m_angle = -0.628f;
	float m_height = 30.0f;
	float m_zoom = 1.0f;
	int m_streamingDistance;
	float m_rimScale = 1.0f;
	float m_fadeValue = 0.0f;
	float m_fadeCircleValue = 1.0f;
	float m_separaionWeight = 3.0f;

	IsometricCamera m_camera;

	Hero* m_hero;
	Bot *m_currentBot;
	Md2Node *m_md2Node;
	ShapeNode *m_segmentNode = nullptr, *m_diskNode = nullptr, *m_buildingNode = nullptr, *m_cursorNode = nullptr;

	SceneNodeLC* m_root;
	Octree* m_octree;
	Frustum m_frustum;
	Scene m_scene;
	MiniMap m_miniMap;
	Billboard m_billboard;
	NavPolygonHelper m_navPolygonHelper;
	CameraController m_cameraController;
	NavigationMesh* m_navigationMesh;
	CrowdManager* m_crowdManager;
	CrowdAgent* m_agent;

	MousePicker m_mousePicker;
	btCollisionObject* m_ground;	
	std::vector<Md2Entity*> m_entities;

	Sprite m_panel;
	Framebuffer m_depthRT, m_shadowRT, m_mainRT;

	Fade m_fade, m_fadeCircle;	
	int m_currentPanelTex;
	std::vector<btCollisionObject*> m_colliosionFilter;

	Matrix4f m_shadowMatrix;
	Vector3f m_lightRight;
};