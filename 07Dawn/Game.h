#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "engine/Camera.h"
#include "engine/Batchrenderer.h"
#include "engine/CharacterSet.h"

#include "StateMachine.h"
#include "ViewPort.h"

#include "Label.h"
#include "Modal.h"

class Game : public State, public MouseEventListener {
public:
	Game(StateMachine& machine);
	~Game();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &frameBuffer) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;

	Zone* newZone;
	Camera m_camera;

	Modal m_modal;
};