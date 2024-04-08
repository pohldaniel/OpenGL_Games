#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/Background.h>
#include <engine/ObjModel.h>
#include <engine/Framebuffer.h>

#include <States/StateMachine.h>

#define DEPTH_TEXTURE_SIZE 2048

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

public:

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

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;
	ObjModel m_statue;

	Framebuffer m_mainRT;
	Framebuffer m_sssXRT;
	Framebuffer m_sssYRT;

	Matrix4f currViewProj;
	Matrix4f prevViewProj;

	bool m_sss = true;
	bool m_showBlurRadius = false;
	bool m_debug = true;
	float m_sssWidth = 20.0f;
	float m_translucency = 0.0f;
	float m_specularIntensity = 0.46f;
	Light lights[4];
};