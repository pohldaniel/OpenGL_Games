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
	void shadowPass(ObjModel& model, const Matrix4f& transform);

	Camera m_camera;
	Cube* m_cube;
	Quad* m_quad;
	Framebuffer m_fbo;
	
	TrackBall m_trackball;
	Transform m_transformBuddha;
	Transform m_transformDragon;

	ObjModel m_buddha;
	ObjModel m_dragon;

	Matrix4f m_shadowView;
	Matrix4f m_shadowProjection;
};

