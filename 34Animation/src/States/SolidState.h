#pragma once
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/animation/AssimpAnimatedModel.h>
#include <engine/MeshObject/Shape.h>
#include <engine/scene/SceneNode.h>
#include <engine/Camera.h>
#include <engine/ObjModel.h>
#include <States/StateMachine.h>
#include <Windows.h>
#include <Utils/SolidIO.h>
#include "Model.h"

class SolidState : public State, public MouseEventListener, public KeyboardEventListener {

public:

	SolidState(StateMachine& machine);
	~SolidState();

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
	bool m_playAnimation = true;
	bool m_togglePose = false;
	bool m_toggleSkin = false;
	Camera m_camera;
	Model m_model;
};