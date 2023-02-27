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
#include "TerrainNV.h"

struct obj_BoundingSphere {
	Vector3f center;
	float radius;
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
	
	bool m_initUi = true;	

	TerrainNV *terrain;
	obj_BoundingSphere obj_BSphere[NUM_OBJECTS];

	void renderScene();
};

