#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/Background.h>
#include <engine/ObjModel.h>
#include <engine/Framebuffer.h>
#include <engine/TrackBall.h>

#include <States/StateMachine.h>
#include "nv_dds.h"



struct Light {
	float fov;
	float falloffWidth;
	Vector3f color;
	float attenuation;
	float farPlane;
	float bias;
	Vector3f pos;
	Vector3f viewDirection;
	Matrix4f m_shadowView;
	Matrix4f m_shadowProjection;
	Framebuffer m_depthRT;
};

class Separable : public State, public MouseEventListener, public KeyboardEventListener {

	static const int DEPTH_TEXTURE_SIZE = 2048;

public:

	enum Model {
		STATUE,
		DRAGON
	};

	Separable(StateMachine& machine);
	~Separable();

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
	void renderGBuffer();
	void shdowPass();
	void sssPass();
	void addSpecular();
	void presetDragon();
	void presetStatue();
	void applyTransformation(TrackBall& arc);

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_tangentSpaceNormal = true;
	bool m_specularAOMap = true;

	Camera m_camera;
	ObjModel m_statue, m_dragon;
	TrackBall m_trackball;
	Transform m_transform;

	Framebuffer m_mainRT;
	Framebuffer m_sssXRT;
	Framebuffer m_sssYRT;

	Matrix4f currViewProj;
	Matrix4f prevViewProj;
	Vector3f m_center;

	bool m_sss = true;
	bool m_addSpecular = true;
	bool m_showBlurRadius = false;
	bool m_debug = false;
	bool m_folowSurface = false;
	bool m_wasFollow = true;
	bool m_shadow = true;

	float m_sssWidth = 50.0f;
	float m_translucency = 0.65f;
	float m_specularIntensity = 1.0f;
	float m_specularRoughness = 0.08f;
	float m_specularFresnel = 0.81f;
	float m_bumpiness = 0.89f;

	float m_weight = 2.0f;
	float m_strength = 10.0f;
	float m_translucencyStrength = 5.0f;

	float m_lightColor[3] = { 1.0f, 1.0f, 1.0f };
	float m_albedoColor[3] = { 1.0f, 1.0f, 1.0f };
	const float clear[1] = { 1.0f};
	float m_spec[4] = { 0.0f, 0.0f, 0.0f, m_strength };
	unsigned int m_specu[4] = { 0, 0, 0, 255 };

	Light lights[4];
	Model model;
	unsigned int sampler1, sampler2, sampler3, sampler4;
};