#pragma once

#include "engine/input/KeyBorad.h"

#include "Constants.h"
#include "StateMachine.h"

#include "ObjModel.h"
#include "Camera.h"
#include "Camera2.h"

class Game : public State {
public:
	Game(StateMachine& machine);
	~Game();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &m_frameBuffer) override;

	Camera camera;
	Camera2 camera2;
	Model *room, *car, *wheel[4], *chairs, *middesk, *sidedesk;
	bool followCamera = true;

	std::vector<Matrix4f> cols;
	std::vector<Matrix4f> middesks;
	std::vector<Matrix4f> sidedesks;
};