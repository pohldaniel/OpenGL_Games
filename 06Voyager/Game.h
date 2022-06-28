#pragma once

#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"

#include "Constants.h"
#include "StateMachine.h"

#include "Camera.h"
#include "Terrain.h"

class Game : public State {
public:
	Game(StateMachine& machine);
	~Game();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render() override;
	void performCameraCollisionDetection();

	Camera m_camera;
	Terrain m_terrain;

	Vector3f m_cameraBoundsMax;
	Vector3f m_cameraBoundsMin;
};