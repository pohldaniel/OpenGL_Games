#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/Keyboard.h"
#include "engine/input/Mouse.h"
#include "engine/Camera.h"
#include "engine/TrackBall.h"
#include "engine/Shader.h"
#include "engine/Vector.h"
#include "engine/Frustum.h"
#include "engine/Framebuffer.h"
#include "engine/Texture.h"
#include "engine/Clock.h"

#include "StateMachine.h"

#include "CloudsModel.h"


enum cloudsTextureNames { fragColor, bloom, alphaness, cloudDistance };

struct colorPreset {
	Vector3f cloudColorBottom, skyColorTop, skyColorBottom, lightColor, fogColor;
};

class Clouds : public State, public MouseEventListener {

public:

	Clouds(StateMachine& machine);
	~Clouds();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void applyTransformation(TrackBall& arc);
	void renderUi();

	Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;
	bool m_initUi = true;

	CloudsModel cloudsModel;
	Vector3f fogColor;
	Vector3f lightColor;

	Vector3f lightPosition;
	Vector3f lightDirection;

	Framebuffer sceneBuffer;
	Framebuffer cloudsBuffer;
	Framebuffer skyBuffer;

	Texture m_textureSet[4];
	Clock m_clock;

	colorPreset DefaultPreset();
	colorPreset SunsetPreset();
	colorPreset SunsetPreset1();

	void mixSkyColorPreset(float v, colorPreset p1, colorPreset p2);
	colorPreset presetSunset, highSunPreset;
	Vector3f skyColorTop, skyColorBottom;
	void updateSky();
};

