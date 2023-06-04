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

#include "buffers.h"
#include "SceneElements.h"
#include "Skybox.h"
#include "CloudsModel.h"
#include "VolumetricClouds.h"

class Clouds : public State, public MouseEventListener {

public:

	Clouds(StateMachine& machine);
	~Clouds();

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

	FrameBufferObject SceneFBO;
	sceneElements scene;
	Camera2 camera;

	Skybox skybox;
	CloudsModel cloudsModel;
	glm::vec3 fogColor;
	glm::vec3 lightColor;

	VolumetricClouds volumetricClouds;
	VolumetricClouds reflectionVolumetricClouds;

	ScreenSpaceShader PostProcessing;
	ScreenSpaceShader fboVisualizer;
};

