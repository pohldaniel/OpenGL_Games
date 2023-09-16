#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"
#include "engine/Camera.h"
#include "engine/TrackBall.h"
#include "StateMachine.h"
#include "Physics.h"
#include "Ragdoll.h"
#include "MousePicker.h"
#include "ShapeDrawer.h"
#include "Background.h"

#include "Particle.hpp"
#include "RigidBody.hpp"
#include "Settings.h"
#include "Networking.hpp"
#include "World.h"
#include "GameDir.hpp"
#include "Controls.hpp"
#include "Shaders.hpp"

#include <SDL.h>

class Game : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Game(StateMachine& machine);
	~Game();

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
	TrackBall m_trackball;
	Transform m_transform;
	
	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_switch = false;

	Background m_background1;
	Background m_background2;

	ctpl::thread_pool* updateDirtyPool = nullptr;
	ctpl::thread_pool* rotateVectorsPool = nullptr;

	int prevWidth = 0;
	int prevHeight = 0;

	int newWidth = 0;
	int newHeight = 0;

	int loadingScreenW = 0;
	int loadingScreenH = 0;
	World* world = nullptr;
	GPU_Target* realTarget = nullptr;
	GPU_Target* target = nullptr;
	GameDir gameDir;

	int mx = 0;
	int my = 0;
	int lastDrawMX = 0;
	int lastDrawMY = 0;
	int lastEraseMX = 0;
	int lastEraseMY = 0;
	int scale = 4;

	int ofsX = 0;
	int ofsY = 0;

	float plPosX = 0;
	float plPosY = 0;

	float camX = 0;
	float camY = 0;

	float desCamX = 0;
	float desCamY = 0;

	float freeCamX = 0;
	float freeCamY = 0;

	float accLoadX = 0;
	float accLoadY = 0;

	int fps = 0;
	int feelsLikeFps = 0;
	long long lastTime = 0;
	long long lastTick = 0;
	long long lastLoadingTick = 0;
	long long now = 0;
	long long startTime = 0;
	long long deltaTime;
	long mspt = 0;
	uint32 loadingOnColor = 0;
	uint32 loadingOffColor = 0;

	GPU_Image* textureObjects = nullptr;
	GPU_Image* textureObjectsBack = nullptr;
	GPU_Image* textureObjectsLQ = nullptr;

	GPU_Image* loadingTexture = nullptr;
	GPU_Image* texture = nullptr;
	GPU_Image* worldTexture = nullptr;
	GPU_Image* lightingTexture = nullptr;
	GPU_Image* emissionTexture = nullptr;
	GPU_Image* textureFire = nullptr;
	GPU_Image* texture2Fire = nullptr;
	GPU_Image* textureFlow = nullptr;
	GPU_Image* textureLayer2 = nullptr;
	GPU_Image* textureBackground = nullptr;
	GPU_Image* textureParticles = nullptr;
	GPU_Image* textureEntities = nullptr;
	GPU_Image* textureEntitiesLQ = nullptr;
	GPU_Image* temperatureMap = nullptr;
	GPU_Image* backgroundImage = nullptr;

	#define frameTimeNum 100
	uint16_t* frameTime = new uint16_t[frameTimeNum];

	void updateFrameEarly();
	void tick();
	void tickChunkLoading();
	void tickPlayer();
	/*void updateFrameLate();
	void renderOverlays();

	void renderEarly();
	void renderLate();

	void renderTemperatureMap(World* world);

	int getAimSolidSurface(int dist);
	int getAimSurface(int dist);*/

	std::vector<unsigned char> pixels;
	unsigned char* pixels_ar = nullptr;
	std::vector<unsigned char> pixelsLayer2;
	unsigned char* pixelsLayer2_ar = nullptr;
	std::vector<unsigned char> pixelsBackground;
	unsigned char* pixelsBackground_ar = nullptr;
	std::vector<unsigned char> pixelsObjects;
	unsigned char* pixelsObjects_ar = nullptr;
	std::vector<unsigned char> pixelsTemp;
	unsigned char* pixelsTemp_ar = nullptr;
	std::vector<unsigned char> pixelsParticles;
	unsigned char* pixelsParticles_ar = nullptr;
	std::vector<unsigned char> pixelsLoading;
	unsigned char* pixelsLoading_ar = nullptr;
	std::vector<unsigned char> pixelsFire;
	unsigned char* pixelsFire_ar = nullptr;
	std::vector<unsigned char> pixelsFlow;
	unsigned char* pixelsFlow_ar = nullptr;
	std::vector<unsigned char> pixelsEmission;
	unsigned char* pixelsEmission_ar = nullptr;

	bool* objectDelete = nullptr;
	long long fadeInStart = 0;
	long long fadeInLength = 0;
	int fadeInWaitFrames = 0;

	long long fadeOutStart = 0;
	long long fadeOutLength = 0;
	int fadeOutWaitFrames = 0;

	int tickTime = 0;
	uint16_t* movingTiles;

	WaterShader* waterShader = nullptr;
	WaterFlowPassShader* waterFlowPassShader = nullptr;
	NewLightingShader* newLightingShader = nullptr;
	float newLightingShader_insideDes = 0.0f;
	float newLightingShader_insideCur = 0.0f;
	FireShader* fireShader = nullptr;
	Fire2Shader* fire2Shader = nullptr;
	void loadShaders();
};
