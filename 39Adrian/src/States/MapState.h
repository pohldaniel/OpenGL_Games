#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/octree/Octree.h>
#include <engine/Camera.h>
#include <engine/Background.h>
#include <engine/Frustum.h>
#include <Physics/Physics.h>
#include <Physics/MousePicker.h>
#include <States/StateMachine.h>
#include <Entities/Md2Entity.h>
#include "CameraNew.h"

#define		MAP_MODEL_HEIGHT_Y	25.0f

class MapState : public State, public MouseEventListener, public KeyboardEventListener {

public:

	MapState(StateMachine& machine);
	~MapState();

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
	bool m_useCulling = true;
	bool m_debugTree = false;
	bool m_debugPhysic = true;

	float m_tileFactor = 8.0f;
	float m_angle = -M_PI_4;
	float m_height = 30.0f;
	float m_zoom = 1.0f;

	Camera m_camera;
	CameraNew m_cameraNew;

	Md2Model m_hero;
	Md2Entity *m_heroEnity;

	SceneNodeLC* m_root;
	Octree* m_octree;
	Frustum m_frustum;

	Matrix4f m_view;
	MousePicker m_mousePicker;
};