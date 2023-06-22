#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"
#include "engine/MeshObject/Shape.h"
#include "engine/Shader.h"
#include "engine/Camera.h"
#include "engine/TrackBall.h"
#include "engine/CubeBuffer.h"

#include "StateMachine.h"
#include "RenderableObject.h"
#include "Terrain.h"
#include "QuadTree.h"
#include "CloudsModel.h"
#include "Sky.h"
#include "Light.h"

enum Mode {
	CLOUD,
	WITHCULLING,
	WITHOUTCULLING,	
};

class EnvironmentInterface : public State, public MouseEventListener, public KeyboardEventListener {

public:

	EnvironmentInterface(StateMachine& machine);
	~EnvironmentInterface();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void applyTransformation(TrackBall& arc);
	void renderUi();

	Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;
	bool m_initUi = true;	
	bool m_updateBuffer = false;
	bool m_drawSphere = true;

	CubeBuffer* m_cubeBufferCloud;
	CubeBuffer* m_cubeBufferLayerd;
	CubeBuffer* m_cubeBufferTerrain;
	CubeBuffer* m_cubeBufferTerrainFC;
	CloudsModel m_cloudsModel;
	Sky m_sky;
	Light m_light;

	QuadTree m_quadTree;
	Terrain m_terrain;
	Mode m_mode = Mode::CLOUD;
};
