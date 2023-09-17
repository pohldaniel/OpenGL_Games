#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <box2d/box2d.h>
#include <fcntl.h>
#include <io.h>
#include <codecvt>

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

#include "world.h"
#include "b2DebugDraw_impl.hpp"
#include "Drawing.hpp"
#include "GameDir.hpp"
#include "Settings.h"
#include "Enums.h"

#define VERSION "0.1.0"

class WaterShader;
class WaterFlowPassShader;
class NewLightingShader;
class FireShader;
class Fire2Shader;

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

	static const int MAX_WIDTH = 1920;
	static const int MAX_HEIGHT = 1080;



	GameState state = LOADING;
	GameState stateAfterLoad = MAIN_MENU;
	int networkMode = -1;
	Client* client = nullptr;
	Server* server = nullptr;

	//CAudioEngine audioEngine;

	void handleWindowSizeChange(int newWidth, int newHeight);

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

#define frameTimeNum 100
	uint16_t* frameTime = new uint16_t[frameTimeNum];

	TTF_Font* font64 = nullptr;
	TTF_Font* font16 = nullptr;
	TTF_Font* font14 = nullptr;

	GPU_Target* realTarget = nullptr;
	GPU_Target* target = nullptr;

	void setDisplayMode(DisplayMode mode);
	void setVSync(bool vsync);
	void setMinimizeOnLostFocus(bool minimize);

	GPU_Image* backgroundImage = nullptr;

	GPU_Image* loadingTexture = nullptr;
	std::vector< unsigned char > pixelsLoading;
	unsigned char* pixelsLoading_ar = nullptr;
	int loadingScreenW = 0;
	int loadingScreenH = 0;

	GPU_Image* worldTexture = nullptr;
	GPU_Image* lightingTexture = nullptr;

	GPU_Image* emissionTexture = nullptr;
	std::vector< unsigned char > pixelsEmission;
	unsigned char* pixelsEmission_ar = nullptr;

	GPU_Image* texture = nullptr;
	std::vector< unsigned char > pixels;
	unsigned char* pixels_ar = nullptr;
	GPU_Image* textureLayer2 = nullptr;
	std::vector< unsigned char > pixelsLayer2;
	unsigned char* pixelsLayer2_ar = nullptr;
	GPU_Image* textureBackground = nullptr;
	std::vector< unsigned char > pixelsBackground;
	unsigned char* pixelsBackground_ar = nullptr;
	GPU_Image* textureObjects = nullptr;
	GPU_Image* textureObjectsLQ = nullptr;
	std::vector< unsigned char > pixelsObjects;
	unsigned char* pixelsObjects_ar = nullptr;
	GPU_Image* textureObjectsBack = nullptr;
	GPU_Image* textureParticles = nullptr;
	std::vector< unsigned char > pixelsParticles;
	unsigned char* pixelsParticles_ar = nullptr;
	GPU_Image* textureEntities = nullptr;
	GPU_Image* textureEntitiesLQ = nullptr;

	GPU_Image* textureFire = nullptr;
	GPU_Image* texture2Fire = nullptr;
	std::vector< unsigned char > pixelsFire;
	unsigned char* pixelsFire_ar = nullptr;

	//GPU_Image* textureFlowSpead = nullptr;
	GPU_Image* textureFlow = nullptr;
	std::vector< unsigned char > pixelsFlow;
	unsigned char* pixelsFlow_ar = nullptr;

	GPU_Image* temperatureMap = nullptr;
	std::vector< unsigned char > pixelsTemp;
	unsigned char* pixelsTemp_ar = nullptr;

	b2DebugDraw_impl* b2DebugDraw;

	int ent_prevLoadZoneX = 0;
	int ent_prevLoadZoneY = 0;
	ctpl::thread_pool* updateDirtyPool = nullptr;
	ctpl::thread_pool* rotateVectorsPool = nullptr;

	uint16_t* movingTiles;
	void updateMaterialSounds();

	int tickTime = 0;

	bool running = true;

	World* world = nullptr;

	float accLoadX = 0;
	float accLoadY = 0;

	int mx = 0;
	int my = 0;
	int lastDrawMX = 0;
	int lastDrawMY = 0;
	int lastEraseMX = 0;
	int lastEraseMY = 0;

	bool* objectDelete = nullptr;

	WaterShader* waterShader = nullptr;
	WaterFlowPassShader* waterFlowPassShader = nullptr;
	NewLightingShader* newLightingShader = nullptr;
	float newLightingShader_insideDes = 0.0f;
	float newLightingShader_insideCur = 0.0f;
	FireShader* fireShader = nullptr;
	Fire2Shader* fire2Shader = nullptr;
	void loadShaders();

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

	DrawTextParams dt_versionInfo1;
	DrawTextParams dt_versionInfo2;
	DrawTextParams dt_versionInfo3;

	DrawTextParams dt_fps;
	DrawTextParams dt_feelsLikeFps;

	DrawTextParams dt_frameGraph[5];
	DrawTextParams dt_loading;

	long long fadeInStart = 0;
	long long fadeInLength = 0;
	int fadeInWaitFrames = 0;

	long long fadeOutStart = 0;
	long long fadeOutLength = 0;
	int fadeOutWaitFrames = 0;
	std::function<void()> fadeOutCallback = []() {};

	GameDir gameDir;

	void init();

	void run();

	void updateFrameEarly();
	void tick();
	void tickChunkLoading();
	void tickPlayer();
	void updateFrameLate();
	void renderOverlays();

	void renderEarly();
	void renderLate();

	void renderTemperatureMap(World* world);

	int getAimSolidSurface(int dist);
	int getAimSurface(int dist);

	void quitToMainMenu();
};
