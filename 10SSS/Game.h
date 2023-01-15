#define NOMINMAX
#pragma once
#include <algorithm>
#include <array>
#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "engine/Framebuffer.h"
#include "engine/Camera.h"
#include "engine/MeshObject/MeshQuad.h"
#include "engine/MeshObject/MeshCube.h"
#include "engine/Cube.h"
#include "engine/ObjModel.h"

#include "Constants.h"
#include "StateMachine.h"
#include "Skybox.h"
#include "Bloom.h"

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

class Game : public State, public MouseEventListener {

public:

	Game(StateMachine& machine);
	~Game();

	void fixedUpdate() override;
	void update() override;
	void render(unsigned int &frameBuffer) override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;	
	void shdowPass();
	void renderGBuffer();
	void sssPass();

	Camera m_camera;

	Framebuffer m_mainRT;
	Framebuffer m_sssXRT;
	Framebuffer m_sssYRT;

	Quad *m_quad;

	bool m_sss = true;
	bool m_showBlurRadius = false;
	bool m_debug = true;
	float m_sssWidth = 20.0f;
	float m_translucency = 0.0f;
	float m_specularIntensity = 0.46f;

	Transform m_transform;
	ObjModel m_statue;

	Light lights[4];
	Matrix4f currViewProj;
	Matrix4f prevViewProj;
	const float red[1] = { 1.0f };
};

