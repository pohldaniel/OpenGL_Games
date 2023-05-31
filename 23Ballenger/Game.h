#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"
#include "engine/Camera.h"
#include "engine/Line.h"

#include "StateMachine.h"
#include "RenderableObject.h"
#include "Terrain.h"
#include "QuadTree.h"

#include "cLava.h"

#include "KeySet.h"
#include "RaySet.h"
#include "RespawnPointSet.h"
#include "ColumnSet.h"
#include "Portal.h"
#include "Player.h"

class Game : public State, public MouseEventListener, public KeyboardEventListener {

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
	void Init();

private:

	int respawn_id, pickedkey_id;
	bool portal_activated;
	float ang;

	cLava Lava;

	Camera m_camera;
	RenderableObject m_skybox;
	RenderableObject m_lava;

	RespawnPointSet m_respawnPointSet;
	KeySet m_keySet;
	RaySet m_raySet;
	
	ColumnSet m_columnSet;
	Portal m_portal;
	Player m_player;

	float m_offsetDistance = 10.0f;
	bool m_initUi = true;
	bool m_drawUi = false;
	int m_depth = 0;

	QuadTree* m_quadTree;
	Terrain m_terrain;
};
