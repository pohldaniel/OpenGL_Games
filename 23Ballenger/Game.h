#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"
#include "engine/Camera.h"
#include "engine/Line.h"

#include "StateMachine.h"
#include "RenderableObject.h"
#include "CharacterController.h"

#include "cPlayer.h"
#include "cRespawnPoint.h"
#include "cColumn.h"
#include "cPortal.h"

#include "Key.h"

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
	std::vector<Vector4f> m_colors;
	std::vector<std::array<bool, 16>> m_activate;

private:

	unsigned char keys[256];
	bool mouse_left_down, mouse_right_down;
	int level, state, respawn_id, pickedkey_id;
	bool noclip, portal_activated;
	float time, ang, noclipSpeedF;
	std::vector<cRespawnPoint> respawn_points;

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

	ThirdPersonCamera m_camera;

	RenderableObject m_sphere;
	RenderableObject m_skybox;
	RenderableObject m_lava;
	RenderableObject m_respawnPoint;
	RenderableObject m_cylinder;
	RenderableObject m_disk;
	RenderableObject m_column;
	RenderableObject m_portal;
	RenderableObject m_energyBallCl;
	RenderableObject m_energyBallP;
	RenderableObject m_vortex;

	Key m_key;
	Line m_line;

	float m_offsetDistance = 10.0f;
	CharacterController *m_characterController;

	std::vector<Vector4f> lineColors;
	std::vector<Vector4f> colors;
	float angle = 0.0f;
	std::vector<unsigned short> pickedIds;
};
