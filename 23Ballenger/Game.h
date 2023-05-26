#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"
#include "engine/Camera.h"
#include "engine/Line.h"

#include "StateMachine.h"
#include "RenderableObject.h"
#include "CharacterController.h"

#include "cPlayer.h"
#include "cPortal.h"

#include "KeySet.h"
#include "RaySet.h"
#include "RespawnPointSet.h"
#include "ColumnSet.h"

#define GRAVITY 0.015f

enum {
	STATE_LIVELOSS,
	STATE_RUN,
	STATE_ENDGAME
};

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
	bool m_initUi = true;
	bool m_drawUi = false;
	bool m_enableSpring = true;
	bool m_useThirdCamera = false;

	bool Init(int lvl);


private:

	int level, state, respawn_id, pickedkey_id;
	bool portal_activated;
	float ang;

	cPortal Portal;
	cScene Scene;
	cData Data;
	cPlayer Player;
	cCamera camera;
	//cSound Sound;
	cShader Shader;
	cModel Model;
	cTerrain Terrain;
	cLava Lava;

	Vector3f m_pos;
	Vector3f m_playerPos;

	ThirdPersonCamera m_camera;
	RenderableObject m_sphere;
	RenderableObject m_skybox;
	RenderableObject m_lava;

	RenderableObject m_column;
	RenderableObject m_portal;
	RenderableObject m_energyBallCl;
	RenderableObject m_energyBallP;
	RenderableObject m_vortex;

	KeySet m_keySet;
	RaySet m_raySet;
	RespawnPointSet m_respawnPointSet;
	ColumnSet m_columnSet;

	float m_offsetDistance = 10.0f;
	CharacterController *m_characterController;


	float angle = 0.0f;

};
