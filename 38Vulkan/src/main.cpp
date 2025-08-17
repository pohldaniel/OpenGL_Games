
#include <windows.h>	
#include <tchar.h>
#include <iostream>
#include <stdlib.h>
#include <engine/Clock.h>

#include "Application.h"
#include "Globals.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

#if DEBUG
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);
	SetConsoleTitle("Debug console");

	MoveWindow(GetConsoleWindow(), 1300, 0, 570, 300, true);
	std::cout << "right mouse       : capture game" << std::endl;
	std::cout << "ESC               : quit" << std::endl;
	std::cout << "v                 : toggle vsync" << std::endl;
	std::cout << "z                 : toggle wireframe" << std::endl;
	std::cout << "right alt + enter : fullscreen" << std::endl;

	int frames = 0;
	float frameTime = 0;
#endif

	float deltaTime = 0.0f;
	float fixedDeltaTime = 0.0f;
	float physicsElapsedTime = 0.0;
	Application application(deltaTime, fixedDeltaTime);

	HWND hwnd = Application::GetWindow();

	Clock deltaClock;
	Clock fixedDeltaClock;

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

	}
	//system("pause");
	return 0;
}