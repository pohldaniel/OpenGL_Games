#pragma once
#include "engine/input/EventDispatcher.h"
#include "engine/input/MouseEventListener.h"
#include "engine/input/Mouse.h"
#include "engine/Transform.h"
#include "StateMachine.h"

class About : public State, public MouseEventListener {

public:
	About(StateMachine& machine);
	~About();

	void fixedUpdate() override;
	void update() override;
	void render() override;

private:

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;

	Transform m_transform;
	bool m_highlight = false;
};