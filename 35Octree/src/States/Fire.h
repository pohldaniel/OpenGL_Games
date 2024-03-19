#pragma once
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/scene/SceneNodeLC.h>
#include <engine/Camera.h>
#include <engine/AssimpModel.h>
#include <engine/ObjModel.h>
#include <States/StateMachine.h>

#include "Entity.h"
#include "Flame.h"


class Fire : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Fire(StateMachine& machine);
	~Fire();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void OnKeyUp(Event::KeyboardEvent& event) override;

private:

	void renderUi();

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_drawOffscreen = false;
	bool m_fireUp;

	Camera m_camera;

	SceneNodeLC* m_root;
	AssimpModel m_logPine, m_grass;
	std::vector<Entity*> entities;
	Vector3f m_lightPosition;
	Flame m_flame;
};