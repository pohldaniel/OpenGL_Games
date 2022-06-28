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
	virtual void render(unsigned int &m_frameBuffer) override;

	Camera camera;
	Terrain m_terrain;
};