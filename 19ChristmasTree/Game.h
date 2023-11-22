#define NOMINMAX
#pragma once
#include <algorithm>
#include <array>
#include <random>

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "engine/Camera.h"
#include "engine/TrackBall.h"
#include "engine/Cube.h"

#include "Constants.h"
#include "StateMachine.h"
#include "XTree.h"

enum Buffer {
	RESULT,
	HFILTER,
	VFILTER,
	NORMAL,
	POSITION,
	MATERIAL
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

	Camera m_camera;

	TrackBall m_trackball;
	Transform m_transform;
	Quad* m_quad;
	Cube* m_cube;
	Cubemap m_cubemap;
	bool m_initUi = true;	

	Shader* basic;
	Shader* ornament;
	Shader* light;
	Shader* defProg;
	Shader* displayProg;
	Shader* filterProg;
	Shader* defPtProg;
	Shader* reflectionProg;
	Shader* quad;
	Shader* sky;
	Texture m_hdriCross;

	XTree tree;
	void drawTreeGeometry();
	void drawOrnamentGeometry();
	void drawLights();
	void drawLighting();
	void preDrawTree();
	void drawPost();

	void loadTextures();
	void createFBO();

	GLuint dummyNormal;
	GLuint frond;
	GLuint frondNormal;
	GLuint brownTex;
	GLuint whiteTex;
	GLuint cubeTex;

	Texture m_needles;
	Texture m_needlesN;

	bool m_drawFronds = true;
	bool m_drawBranches = true;
	bool m_drawOrnaments = true;
	bool m_drawLights = true;
	bool m_drawReflections = true;
	bool m_intermediate = false;
	bool m_drawSkaybox = false;
	//
	//fbo data
	//
	GLuint deferredShadingFBO = 0, lightAccumFBO = 0, horFilterFBO = 0, vertFilterFBO = 0;
	GLuint normal = 0, pos = 0, material = 0, result = 0, hFilter = 0, vFilter = 0;
	GLuint depthBuffer = 0;
	GLenum hdrFormat = GL_R11F_G11F_B10F_EXT;
	int dsLevel = 1;

	int currentTime = 0;
	const int numTimerSamples = 120;

	//values used in uniforms
	float exposure = 0.125f;
	float sigmaVal = 1.5f;
	float blurVal = 0.4f;

	Buffer m_buffer = Buffer::RESULT;
	GLuint m_displayTex;
};

