#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/TrackBall.h>
#include <engine/Frustum.h>
#include <engine/scene/ShapeNode.h>
#include <engine/octree/Octree.h>

#include <States/StateMachine.h>

class OctreeInterface : public State, public MouseEventListener, public KeyboardEventListener {

public:

	OctreeInterface(StateMachine& machine);
	~OctreeInterface();

	void fixedUpdate() override;
	void update() override;
	void render() override;

	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;

private:

	void renderUi();
	Camera m_camera;

	bool m_initUi = true;
	bool m_drawUi = true;

	float m_fovx = 44.0f;
	float m_far = 70.0f;
	float m_near = 5.0f;
	float m_distance = 0.01f;
	bool m_overview = true;
	bool m_useCulling = true;
	bool m_useOcclusion = true;

	SceneNodeLC* m_root;
	std::vector<ShapeNode*> m_entities;
	Octree* m_octree;
	Frustum m_frustum;
	Matrix4f perspective, m_view;
};