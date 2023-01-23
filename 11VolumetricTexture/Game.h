#define NOMINMAX
#pragma once
#include <algorithm>
#include <array>
#include <random>
#include <nvGlutManipulators.h>

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"

#include "engine/MeshObject/MeshQuad.h"
#include "engine/MeshObject/MeshCube.h"
#include "engine/Framebuffer.h"
#include "engine/Camera.h"
#include "engine/ObjModel.h"
#include "engine/Cube.h"

#include "Constants.h"
#include "StateMachine.h"
#include "FluidSystem.h"

typedef std::vector<Vector3f> PointList;

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
	void applyTransformation(nv::matrix4f& mtx);
	unsigned int createPyroclasticVolume(int n, float r);
	unsigned int createPointVbo(float x, float y, float z);
	void renderOffscreen();

	Camera m_camera;
	Cube* m_cube;
	Quad* m_quad;

	int m_depth = 128, m_width = 128, m_height = 128;
	FluidSystem *fluidSys;

	nv::GlutExamine trackball;
	Matrix4f m_model;
	Matrix4f m_invModel;

	Transform m_tranformSplat;
	Transform m_tranformFluid;
	Transform m_tranformCloud1;
	Transform m_tranformCloud2;
	Transform m_tranformModel;

	unsigned int m_result;
	unsigned int cloudTexture; 
	unsigned int cubeCenterVbo;

	Framebuffer m_intervalls[2];
	Framebuffer m_surface;

	const int PositionSlot = 0;
	const int GridDensity = 16;

	GLuint CreatePoints();
	GLuint CreateTexture(const char *filename, int width, int height, int depth, bool flipvertical = true);
	GLuint Create(const char *path, int width, int height, int slices, bool flipvertical = true);

	GLuint CreateSplat(PointList positions);
	GLuint CreateCpuSplat();

	PointList CreatePathline();

	GLuint GridVao;
	GLuint SplatTexture;
	GLuint SplatTextureCpu;
	GLuint scan[3];

	bool m_cpuSplat = false;
	bool m_drawBorder = true;

	int m_currentModel = 0;
};

