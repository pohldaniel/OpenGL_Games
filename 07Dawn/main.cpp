#define NOMINMAX
#include <windows.h>	
#include <tchar.h>
#include <iostream>
#include <stdlib.h>
#include "engine\Clock.h"
#include "Application.h"
#include "Constants.h"


extern Matrix4f Globals::projection = Matrix4f();
extern Matrix4f Globals::invProjection = Matrix4f();
extern Matrix4f Globals::orthographic = Matrix4f();
extern Matrix4f Globals::invOrthographic = Matrix4f();
extern AssetManager<Shader> Globals::shaderManager = AssetManager<Shader>();
extern AssetManager<Texture> Globals::textureManager = AssetManager<Texture>();
extern AssetManager<Spritesheet> Globals::spritesheetManager = AssetManager<Spritesheet>();
extern AssetManager<CharacterSet> Globals::fontManager = AssetManager<CharacterSet>();
extern bool Globals::enableWireframe = false;
extern std::map<std::string, Zone*> Globals::allZones = std::map<std::string, Zone*>();
extern Zone* Globals::currentZone = nullptr;
extern bool  Globals::initPhase = false;
extern unsigned int Globals::textureAtlas = 0;

extern void Globals::setCurrentZone(Zone* newCurZone){
	Globals::currentZone = newCurZone;
}

extern Zone* Globals::getCurrentZone()
{
	return Globals::currentZone;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	#if DEBUG
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	SetConsoleTitle("Debug console");

	MoveWindow(GetConsoleWindow(), 1300, 0, 550, 300, true);
	std::cout << "w, a, s, d : scroll map" << std::endl;
	#endif
	
	float deltaTime = 0.0f;
	float fixedDeltaTime = 0.0f;
	float physicsElapsedTime = 0.0;

	Application application(deltaTime, fixedDeltaTime);

	HWND hwnd = application.getWindow();
	HDC hdc;

	Clock deltaClock;
	Clock fixedDeltaClock;

	int frames = 0;
	float framesTime = 0;

	while (application.isRunning()) {
		physicsElapsedTime += deltaTime;
		while (physicsElapsedTime > PHYSICS_STEP) {
			fixedDeltaTime = fixedDeltaClock.restartSec();
			if (fixedDeltaTime > PHYSICS_STEP * 5.0f)
				fixedDeltaTime = PHYSICS_STEP;

			application.fixedUpdate();
			physicsElapsedTime -= PHYSICS_STEP;
		}

		application.update();
		application.render();


		deltaTime = deltaClock.restartSec();

		#if DEBUG
		framesTime += deltaTime;
		frames++;
		if (framesTime > 1) {
			_TCHAR fpsText[32];
			_sntprintf(fpsText, 32, "FPS: %d FPS", frames);
			SetWindowText(hwnd, fpsText);
			frames = 0;
			framesTime = 0;
			deltaClock.restart();
		}


		#endif
		hdc = GetDC(hwnd);
		SwapBuffers(hdc);
		ReleaseDC(hwnd, hdc);
	}
	
}