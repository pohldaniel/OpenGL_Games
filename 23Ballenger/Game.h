#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"

#include "StateMachine.h"
#include "cPlayer.h"
#include "cRespawnPoint.h"
#include "cColumn.h"
#include "cPortal.h"
#include "cKey.h"

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
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void renderUi();
	bool m_initUi = true;

	bool Init(int lvl);

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
	cCamera Camera;
	//cSound Sound;
	cShader Shader;
	cModel Model;
	cTerrain Terrain;
	cLava Lava;

	//void Physics(cBicho &object);
};
