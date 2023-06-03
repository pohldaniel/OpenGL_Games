#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/Keyboard.h"
#include "engine/input/Mouse.h"
#include "engine/Camera.h"
#include "engine/TrackBall.h"
#include "engine/Shader.h"
#include "engine/Vector.h"

#include "StateMachine.h"

#include "Terrain.h"
#include "QuadTree_new.h"



// ----------------------------------------------------------------------------------------------------------------------------

class Culling : public State, public MouseEventListener {

public:

	Culling(StateMachine& machine);
	~Culling();

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
	bool m_overview;

	bool Wireframe, RenderAABB, VisualizeRenderingOrder, SortVisibleGeometryNodes, VisibilityCheckingPerformanceTest;
	int Depth;

	Terrain m_terrain;
	QuadTreeNew m_quadTree;
	Matrix4f m_view, m_orthographic;
};

