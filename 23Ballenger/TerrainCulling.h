#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/Keyboard.h"
#include "engine/input/Mouse.h"
#include "engine/Camera.h"
#include "engine/TrackBall.h"
#include "engine/Shader.h"
#include "engine/Vector.h"
#include "engine/Frustum.h"

#include "StateMachine.h"

#include "Terrain.h"
#include "QuadTree.h"

class TerrainCulling : public State, public MouseEventListener {

public:

	TerrainCulling(StateMachine& machine);
	~TerrainCulling();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void applyTransformation(TrackBall& arc);
	void renderUi();

	Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;
	bool m_initUi = true;

	bool m_useCulling = true;
	bool m_overview = true;
	bool m_showDrawOrder = true;
	bool m_sortVisibility = true;
	bool m_drawAABB = true;
	int m_depth = -1;

	float m_fovx = 44.0f;
	float m_far = 990.0f;
	float m_near = 5.0f;
	float m_distance = 0.0f;

	Terrain m_terrain;
	QuadTree m_quadTree;
	Matrix4f m_view, m_orthographic, m_perspective;
};

