#include <windows.h>	
#include <tchar.h>
#include <iostream>
#include <stdlib.h>
#include "engine\Clock.h"
#include "Application.h"
#include "Globals.h"

extern AssetManager<Shader> Globals::shaderManager = AssetManager<Shader>();
extern AssetManager<Texture> Globals::textureManager = AssetManager<Texture>();
extern AssetManager<Shape> Globals::shapeManager = AssetManager<Shape>();
extern AssetManager<CharacterSet> Globals::fontManager = AssetManager<CharacterSet>();
extern AssetManager<Spritesheet> Globals::spritesheetManager = AssetManager<Spritesheet>();
extern AssetManager<SoundBuffer> Globals::soundManager = AssetManager<SoundBuffer>();
extern AssetManager<MusicBuffer> Globals::musicManager = AssetManager<MusicBuffer>();

extern float Globals::soundVolume = 0.2f;
extern float Globals::musicVolume = 0.2f;

extern Physics* Globals::physics = NULL;

extern unsigned int Globals::colorUbo = 0;
extern unsigned int Globals::activateUbo = 0;

extern const unsigned int Globals::colorBinding = 0;
extern const unsigned int Globals::activateBinding = 1;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

#if DEBUG
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	SetConsoleTitle("Debug console");

	MoveWindow(GetConsoleWindow(), 1300, 0, 550, 300, true);
	std::cout << "right mouse      : capture game" << std::endl;
	std::cout << "space            : capture ui" << std::endl;
	std::cout << "ESC              : quit" << std::endl;
	std::cout << "v                : toggle vsync" << std::endl;
	std::cout << "alt + enter      : fullscreen" << std::endl;

	int frames = 0;
	float framesTime = 0;
#endif
	Globals::physics = new Physics(PHYSICS_STEP);
	float deltaTime = 0.0f;
	float fixedDeltaTime = 0.0f;
	float physicsElapsedTime = 0.0;
	Application application(deltaTime, fixedDeltaTime);

	HWND hwnd = Application::GetWindow();
	HDC hdc;

	Clock deltaClock;
	Clock fixedDeltaClock;
      
	while (application.isRunning()) {

		physicsElapsedTime += deltaTime;
		while (physicsElapsedTime > PHYSICS_STEP) {
			fixedDeltaTime = fixedDeltaClock.resetSec();
			if (fixedDeltaTime > PHYSICS_STEP * 5.0f)
				fixedDeltaTime = PHYSICS_STEP;

			application.fixedUpdate();
			physicsElapsedTime -= PHYSICS_STEP;
		}

		
		application.update();
		application.render();

		deltaTime = deltaClock.resetSec();

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

	delete Globals::physics;
	return 0;
}
