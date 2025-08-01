#include <windows.h>	
#include <tchar.h>
#include <iostream>
#include <stdlib.h>
#include <engine/Clock.h>

#include "Application.h"
#include "Globals.h"

AssetManager<Shader> Globals::shaderManager = AssetManager<Shader>();
AssetManager<Texture> Globals::textureManager = AssetManager<Texture>();
AssetManager<Shape> Globals::shapeManager = AssetManager<Shape>();
AssetManager<CharacterSet> Globals::fontManager = AssetManager<CharacterSet>();
AssetManager<Spritesheet> Globals::spritesheetManager = AssetManager<Spritesheet>();
AssetManager<SoundBuffer> Globals::soundManager = AssetManager<SoundBuffer>();
AssetManager<MusicBuffer> Globals::musicManager = AssetManager<MusicBuffer>();
AssetManager<AssimpAnimation> Globals::animationManager = AssetManager<AssimpAnimation>();AssetManager<Animation> Globals::animationManagerNew = AssetManager<Animation>();

extern std::unique_ptr<Physics> Globals::physics = nullptr;
extern Clock Globals::clock = Clock();

extern unsigned int Globals::lightUbo = 0;
extern const unsigned int Globals::lightBinding = 0;

extern float Globals::soundVolume = 0.2f;
extern float Globals::musicVolume = 0.05f;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

#if DEBUG
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);
	SetConsoleTitle("Debug console");

	MoveWindow(GetConsoleWindow(), 1300, 0, 570, 300, true);
	std::cout << "right mouse       : capture game" << std::endl;
	std::cout << "left alt          : toggle ui" << std::endl;
	std::cout << "ESC               : quit" << std::endl;
	std::cout << "v                 : toggle vsync" << std::endl;
	std::cout << "z                 : toggle wireframe" << std::endl;
	std::cout << "right alt + enter : fullscreen" << std::endl;

	int frames = 0;
	float frameTime = 0;
#endif

	Globals::physics = std::make_unique<Physics>(PHYSICS_STEP);

	float deltaTime = 0.0f;
	float fixedDeltaTime = 0.0f;
	float physicsElapsedTime = 0.0;
	Application application(deltaTime, fixedDeltaTime);

	HWND hwnd = Application::GetWindow();
	HDC hdc;

	Clock deltaClock;
	Clock fixedDeltaClock;
	Globals::clock.restart();

	while (application.isRunning()) {
		physicsElapsedTime += deltaTime;
		while (physicsElapsedTime > PHYSICS_STEP) {
			fixedDeltaTime = fixedDeltaClock.resetSec();
			if (fixedDeltaTime > PHYSICS_STEP)
				fixedDeltaTime = PHYSICS_STEP;

			application.fixedUpdate();
			physicsElapsedTime -= PHYSICS_STEP;
		}


		application.update();
		application.render();

		//if (deltaTime > 1.0f) {
		//	deltaTime = UPDATE_STEP;
		//}

		//float timeToSleep = UPDATE_STEP - deltaTime;
		//if (0 < timeToSleep) {
		//	std::this_thread::sleep_for(std::chrono::milliseconds((uint32_t)(timeToSleep * 1000.0f)));
		//}

		deltaTime = deltaClock.resetSec();

#if DEBUG
		frameTime += deltaTime;
		frames++;
		if (frameTime > 1) {
			_TCHAR fpsText[32];
			_sntprintf(fpsText, 32, "FPS: %d FPS", frames);
			SetWindowText(hwnd, fpsText);
			frames = 0;
			frameTime = 0;
			deltaClock.restart();
		}
#endif

		hdc = GetDC(hwnd);
		SwapBuffers(hdc);
		ReleaseDC(hwnd, hdc);
	}
	return 0;
}