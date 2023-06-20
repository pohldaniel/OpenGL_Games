#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"
#include "engine/Camera.h"
#include "engine/Line.h"
#include "engine/CubeBuffer.h"
#include "Physics.h"
#include "StateMachine.h"
#include "RenderableObject.h"
#include "Terrain.h"
#include "QuadTree.h"

#include "KeySet.h"
#include "RaySet.h"
#include "RespawnPointSet.h"
#include "ColumnSet.h"
#include "Portal.h"
#include "Player.h"
#include "Lava.h"

#include "CloudsModel.h"
#include "Sky.h"
#include "Light.h"

#include "StaticTrigger.h"

class Game;

struct LavaTriggerCallback : public btCollisionWorld::ContactResultCallback {

	LavaTriggerCallback(KeySet& keySet, const RespawnPointSet& respawnPointSet) : keySet(keySet), respawnPointSet(respawnPointSet){}
	KeySet& keySet;
	const RespawnPointSet& respawnPointSet;

	btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override {
		Player* player = reinterpret_cast<Player*>(colObj0Wrap->getCollisionObject()->getUserPointer());
		player->setPosition(respawnPointSet.getActivePoistion());
		player->resetOrientation();
		keySet.restorePrevState();
		return 0;
	}
};

struct PortalTriggerCallback : public btCollisionWorld::ContactResultCallback {

	PortalTriggerCallback(Game& game) : game(game){}
	Game& game;

	btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override;
};

class Game : public State, public MouseEventListener, public KeyboardEventListener {

	friend struct PortalTriggerCallback;

public:

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
	void init();

private:

	Camera m_camera;
	RenderableObject m_skybox;

	RespawnPointSet m_respawnPointSet;
	ColumnSet m_columnSet;
	KeySet m_keySet;
	RaySet m_raySet;
	Portal m_portal;
	Player m_player;
	Lava m_lava;

	float m_offsetDistance = 10.0f;
	bool m_initUi = true;
	bool m_drawUi = false;
	bool m_useSkybox = false;
	QuadTree m_quadTree;
	Terrain m_terrain;

	CloudsModel m_cloudsModel;
	Sky m_sky;
	Light m_light;
	
	Vector3f m_fogColor;
	Framebuffer sceneBuffer;

	LavaTriggerCallback m_lavaTriggerResult;
	PortalTriggerCallback m_portalTriggerResult;

	int& pickedKeyId;
	bool m_portalActivated;
	StaticTrigger m_ground;
};
