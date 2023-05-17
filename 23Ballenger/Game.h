#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"
#include "engine/Camera.h"

#include "StateMachine.h"
#include "RenderableObject.h"
#include "DynamicCharacterController.h"

#include "cPlayer.h"
#include "cRespawnPoint.h"
#include "cColumn.h"
#include "cPortal.h"
#include "cKey.h"

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
	bool Init(int lvl);

	//Camera m_camera;
	ThirdPersonCamera m_camera2;

	RenderableObject m_sphere;
	float m_offsetDistance = 10.0f;
	DynamicCharacterController *m_dynamicCharacterController;
	btRigidBody* m_rigidBody;
private:
	unsigned char keys[256];
	bool mouse_left_down, mouse_right_down;
	int level, state, respawn_id, pickedkey_id;
	bool noclip, portal_activated;
	float time, ang, noclipSpeedF;
	std::vector<cRespawnPoint> respawn_points;
	std::vector<cKey> target_keys;
	std::vector<cColumn> columns;
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

	void Physics(cBicho &object);
};
