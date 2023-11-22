#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "engine/Camera.h"
#include "engine/TrackBall.h"

#include "StateMachine.h"
#include "Math/XYZ.hpp"

enum drawmodes{
	normalmode,
	motionblurmode,
	radialzoommode,
	realmotionblurmode,
	doublevisionmode,
	glowmode,
};

class Tutorial : public State, public MouseEventListener {

public:

	Tutorial(StateMachine& machine);
	~Tutorial();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	
	void renderUi();
	void DoMouse();

	bool m_initUi = true;

	int numboundaries = 0;
	XYZ boundary[360];
	int change = 0;
	int drawtoggle = 0;

	XYZ terrainlight;
	float terrainheight;
	float distance;

	float updatedelaychange;
	float morphness;
	float framemult;

	XYZ mid;
	float M[16];
	int k;
	int weaponattachmuscle;
	int weaponrotatemuscle;
	XYZ weaponpoint;
	int start, endthing;
};
