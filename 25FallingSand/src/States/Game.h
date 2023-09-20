#pragma once

#include <box2d/box2d.h>
#include <fcntl.h>
#include <io.h>
#include <codecvt>

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"
#include "engine/Camera.h"
#include "engine/TrackBall.h"
#include "engine/Clock.h"
#include "engine/Pixelbuffer.h"

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
	bool m_switch = true;

	Background m_background1;
	Background m_background2;
	Pixelbuffer m_pixelbuffer;
	Texture m_texture;

	float zoomX = 0.25f;
	float offsetX = 0.1f;

	float zoomY = 0.3f;
	float offsetY = 0.05f;

	void init();
	void updateFrameEarly();
	void tick();
	void tickChunkLoading();
	void updateFrameLate();
	void renderTemperatureMap(World* world);

	int getAimSolidSurface(int dist);
	int getAimSurface(int dist);
	void handleWindowSizeChange(int newWidth, int newHeight);

	static const int MAX_WIDTH = 1920;
	static const int MAX_HEIGHT = 1080;

	GameState state = INGAME;
	GameState stateAfterLoad = MAIN_MENU;

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

	int loadingScreenW = 0;
	int loadingScreenH = 0;

	std::vector< unsigned char > pixelsEmission;
	unsigned char* pixelsEmission_ar = nullptr;


	std::vector< unsigned char > pixels;
	unsigned char* pixels_ar = nullptr;

	std::vector< unsigned char > pixelsLayer2;
	unsigned char* pixelsLayer2_ar = nullptr;

	std::vector< unsigned char > pixelsBackground;
	unsigned char* pixelsBackground_ar = nullptr;

	std::vector< unsigned char > pixelsObjects;
	unsigned char* pixelsObjects_ar = nullptr;

	std::vector< unsigned char > pixelsParticles;
	unsigned char* pixelsParticles_ar = nullptr;

	std::vector< unsigned char > pixelsFire;
	unsigned char* pixelsFire_ar = nullptr;

	std::vector< unsigned char > pixelsFlow;
	unsigned char* pixelsFlow_ar = nullptr;

	std::vector< unsigned char > pixelsTemp;
	unsigned char* pixelsTemp_ar = nullptr;

	b2DebugDraw_impl* b2DebugDraw;

	int ent_prevLoadZoneX = 0;
	int ent_prevLoadZoneY = 0;
	ctpl::thread_pool* updateDirtyPool = nullptr;
	ctpl::thread_pool* rotateVectorsPool = nullptr;

	uint16_t* movingTiles;
	World* world = nullptr;

	float accLoadX = 0;
	float accLoadY = 0;

	int mx = 0;
	int my = 0;
	int lastDrawMX = 0;
	int lastDrawMY = 0;
	int lastEraseMX = 0;
	int lastEraseMY = 0;
	int tickTime = 0;

	bool* objectDelete = nullptr;

	GameDir gameDir;
};
