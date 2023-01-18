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

	Camera m_camera;
	Cube* m_cube;

	int m_depth = 128, m_width = 128, m_height = 128;
	FluidSystem *fluidSys;

	nv::GlutExamine trackball;
	Matrix4f m_model;
	Matrix4f m_invModel;
};

