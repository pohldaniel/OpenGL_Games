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

#include "CameraController.h"

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

struct EditPolygon {
	int userPointerOffset = 0;
	int size = 0;
	std::vector<Vector3f> edgePoints;
};

class Adrian : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Adrian(StateMachine& machine, std::string background = "data/textures/buildings/ground.tga");
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
	void loadBillboards();
	bool loadPolygonCache(NavigationMesh* navigationMesh);

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
	void setTarget(const Vector3f& pos);
	const std::vector<std::array<float, 6>>& updateEntitiePositions();
	void createCollisionFilter();
	void activateHero();
	void centerHero();

	bool m_initUi = true;
	bool m_drawUi = false;
	bool m_useCulling = true;
	bool m_debugTree = false;
	bool m_debugPhysic = false;
	bool m_debugNavmesh = false;
	bool m_useStreaming = false;
	bool m_drawPolygon = false;
	bool m_invisible = false;
	bool m_showHelp = false;
	bool m_showPanel = true;
	bool m_noWalls = false;
	bool m_roatecamera = false;
	bool m_fixedUpdate = false;

	float m_tileFactor = 80.0f;
	float m_angle = -0.628f;
	float m_height = 30.0f;
	float m_zoom = 1.0f;
	int m_streamingDistance;
	float m_markerSize = 20.0f;
	float m_rimScale = 1.0f;
	float m_fadeValue = 0.0f;
	float m_fadeCircleValue = 1.0f;
	float m_separaionWeight = 3.0f;

	IsometricCamera m_camera;

	Md2Model m_heroModel, m_hueteotl, m_mutantman, m_corpse, m_mutantlizard, m_mutantcheetah, m_ripper;

	Hero* m_hero;
	Bot *m_currentBot;
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
	std::vector<btCollisionObject*> m_colliosionFilter;

	int m_globalUserIndex;
	Framebuffer m_depthBuffer;
	Fade m_fade, m_fadeCircle;

	std::vector<Md2Entity*> m_entities;
	Sprite m_panel;
	std::vector<TextureRect> m_tileSet;
	unsigned int m_atlas;
	int m_currentPanelTex;
	CharacterSet set;

	std::vector<Vector3f> m_positions;
	unsigned int m_vao = 0u;
	unsigned int m_vbo = 0u;

	std::vector<std::array<float,4>> m_buildings_;
	float m_xconvfactor;
	float m_yconvfactor;
	std::vector<std::array<float, 6>> m_entities_;
	unsigned int m_vao2 = 0u;
	unsigned int m_vbo2 = 0u;
	bool isMouseOver(int sx, int sy, float &, float &);
	unsigned short m_wait;

	std::vector<Bot*> m_bots;
	std::tuple<std::string, std::string> labels[7] = { 
		{"RIGHT CLICK ON ENEMYS TORSO", "TO KILL IT"},
		{"THE CORPSE", "SLOWER THAN HERO"},
		{"MUTANT CHEETA", "ITS FAST ENOUGH TO BE KILLED"},
		{"MUTANT LIZARD", "SLOWER THAN MUTANT MAN"},
		{"MUTANT MAN", "AS FAST AS HERO"},
		{"RIPPER", "VERY FAST TRY SNEAKING PAST IT"},
		{"THE UNDEAD", "VERY SLOW MUTANT"}
	};

	Texture m_texture1, m_texture2, m_background;
	CameraController m_cameraController;
};