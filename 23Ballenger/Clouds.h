#pragma once
#include <GL/glew.h>
#include "engine/input/MouseEventListener.h"
#include "engine/input/Keyboard.h"
#include "engine/input/Mouse.h"
#include "engine/Camera.h"
#include "engine/TrackBall.h"
#include "engine/Shader.h"
#include "engine/Vector.h"
#include "engine/Frustum.h"
#include "engine/Framebuffer.h"
#include "engine/Texture.h"
#include "engine/Clock.h"
#include "engine/SlicedCube.h"
#include "engine/VolumeBuffer.h"
#include "engine/ArrayBuffer.h"

#include "StateMachine.h"
#include "CloudsModel.h"
#include "NoiseGen.h"
#include "Terrain.h"

#include "Light.h"

enum Noise {
	PERLINCOMP,
	WORLEYCOMP,		
	PERLINVERT,
	WORLEYVERT,
	PERLINCPU,
	WORLEYCPU,
	CLOUDS,
	SDF
};





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

	CloudsModel m_cloudsModel;
	Vector3f m_fogColor;

	Framebuffer sceneBuffer;

	Framebuffer rmTarget;


	Clock m_clock;

	



	SlicedCube m_slicedCube;

	bool m_showNoise = false;
	bool m_showWeatherMap = false;
	bool m_showQuad = false;
	bool m_showNoiseArray = false;
	bool m_showSDFArray = false;
	bool m_showCloud = false;
	bool m_drawScene = true;
	Noise m_noise = Noise::PERLINCOMP;
	Matrix4f m_orthographic;

	NoiseGen m_noiseGen;

	unsigned int texture1;
	unsigned int texture2;
	unsigned int worley, perlinworley;
	unsigned int cloudsTex;
	unsigned int sdfTex;
	unsigned int cloudsTo3D;
	unsigned int sdfTo3D;
	int frame = 0;

	VolumeBuffer* m_volumeBuffer = nullptr;
	ArrayBuffer* m_arrayBuffer = nullptr;
	int m_currentArrayIndex = 0;
	Shader* m_cloudGenerator;
	Shader* m_sdfGenerator;
	Shader* m_post;
	Shader* m_raymarch;
	ArrayBuffer* m_buffer = nullptr;
	Texture m_blueNoise;

	Terrain m_terrain;
	Light m_light;
	Sky m_sky;
};

