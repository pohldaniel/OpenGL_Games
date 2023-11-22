#define NOMINMAX
#pragma once
#include <algorithm>
#include <array>
#include <random>

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "engine/Camera.h"
#include "engine/ObjModel.h"
#include "engine/TrackBall.h"

#include "Constants.h"
#include "StateMachine.h"

enum RenderMode {
	TESS,
	EVALUATERS,
	STATIC
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
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void renderUi();
	void applyTransformation(TrackBall& mtx);
	void drawModel();

	Camera m_camera;
	ObjModel m_teapot;
	TrackBall m_trackball;
	Transform m_transform;
	Transform m_transformControl;

	Shader* tess;
	Shader* stat;
	Shader* eva;

	bool m_initUi = true;

	unsigned int viewUbo;
	unsigned int viewBinding = 0;

	void initUniformBuffers();
	void initPatchGeometry(int density);

	GLuint s_vbHandle = 0;
	GLuint s_ibHandle = 0;
	int s_patchVertexCount;
	int s_patchIndexCount;
	bool m_preWarmCache = true;
	int c_teapotPatchCount = 32;

	unsigned int ibo;
	unsigned int vbo;
	Vector3f m_center = Vector3f(0.2625f, 0.0f, 1.575f);

	int m_tessLevel = 9;
	bool m_drawControl = false;
	bool m_wireframe = false;
	RenderMode renderMode = RenderMode::EVALUATERS;
};

