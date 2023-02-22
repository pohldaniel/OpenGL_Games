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

#include "engine/MeshObject/MeshTorus.h"
#include "engine/MeshObject/MeshCylinder.h"

#include "Constants.h"
#include "StateMachine.h"


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

	Shader* lineProg;
	Shader* instanceProg;
	MeshTorus* m_torus;
	MeshCylinder* m_cone;
	MeshCylinder* m_coneTop;
	MeshCylinder* m_coneBottom;

	bool m_initUi = true;	
	
	GLfloat* genLineSphere(int sphslices, int* bufferSize, float scale);
	int	m_sphereBufferSize;
	GLuint m_sphereVBO;
	float g_curSSSize = 1.0f;
};

