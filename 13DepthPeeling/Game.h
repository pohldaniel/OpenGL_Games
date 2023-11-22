#define NOMINMAX
#pragma once
#include <algorithm>
#include <array>
#include <random>

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"

#include "engine/MeshObject/MeshQuad.h"
#include "engine/MeshObject/MeshCube.h"
#include "engine/Framebuffer.h"
#include "engine/Camera.h"
#include "engine/ObjModel.h"
#include "engine/Cube.h"
#include "engine/TrackBall.h"

#include "Constants.h"
#include "StateMachine.h"
#include "FluidSystem.h"

#define MAX_DEPTH 1.0

enum RenderMode {
	DUAL_PEELING_MODE,
	F2B_PEELING_MODE,
	WEIGHTED_AVERAGE_MODE,
	WEIGHTED_SUM_MODE
};

enum Model {
	BUNNY,
	DRAGON,
	BUDDHA
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
	void applyTransformation(Matrix4f& mtx);
	void drawModel();

	void InitF2BPeelingRenderTargets();
	void InitDualPeelingRenderTargets();
	void InitAccumulationRenderTargets();

	void RenderFrontToBackPeeling();
	void RenderDualPeeling();
	void RenderAverageColors();
	void RenderWeightedSum();
	void RenderFullscreenQuad();

	Camera m_camera;
	Quad* m_quad;

	TrackBall m_trackball;
	Transform m_transform;

	ObjModel m_bunny;
	ObjModel m_dragon;
	ObjModel m_buddha;

	Model model = Model::BUNNY;
	RenderMode renderMode = RenderMode::DUAL_PEELING_MODE;
	float m_opacity = 0.6f;
	int m_numPasses = 4;
	bool g_useOQ = true;

	GLuint m_frontFboId[2];
	GLuint m_frontDepthTexId[2];
	GLuint m_frontColorTexId[2];
	GLuint m_frontColorBlenderTexId;
	GLuint m_frontColorBlenderFboId;

	GLuint m_vboId;
	GLuint m_eboId;
	GLuint m_queryId;

	uint32_t m_mode;
	uint32_t m_numGeoPasses;

	Shader* m_shaderF2BInit;
	Shader* m_shaderF2BPeel;
	Shader* m_shaderF2BBlend;
	Shader* m_shaderF2BFinal;

	/////////////////////////////////////////////////
	GLuint g_dualBackBlenderFboId;
	GLuint g_dualPeelingSingleFboId;
	GLuint g_dualDepthTexId[2];
	GLuint g_dualFrontBlenderTexId[2];
	GLuint g_dualBackTempTexId[2];
	GLuint g_dualBackBlenderTexId;

	Shader* m_shaderDualInit;
	Shader* m_shaderDualPeel;
	Shader* m_shaderDualBlend;
	Shader* m_shaderDualFinal;

	////////////////////////////////////////////////////	
	GLuint g_accumulationTexId[2];
	GLuint g_accumulationFboId;
	Shader* m_shaderAverageInit;
	Shader* m_shaderAverageFinal;

	////////////////////////////////////////////////////	
	Shader* m_shaderWeightedInit;
	Shader* m_shaderWeightedFinal;
};

