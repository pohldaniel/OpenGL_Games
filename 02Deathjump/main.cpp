#define NOMINMAX
#define STB_IMAGE_IMPLEMENTATION
#include <windows.h>			
#include "stb\stb_image.h"

#include "Clock.h"
#include "Application.h"


extern float Globals::offset = 0.0f;
extern unsigned long Globals::CONTROLLS = 0;
extern unsigned long Globals::CONTROLLSHOLD = 0;
extern unsigned char Globals::pKeyBuffer[256] = {0};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	SetConsoleTitle("Debug console");

	MoveWindow(GetConsoleWindow(), 1300, 0, 550, 300, true);
	std::cout << "w, a, s, d    : move character" << std::endl;
	std::cout << "q             : change state" << std::endl;
	std::cout << "v             : toggle vsync" << std::endl;
	std::cout << "z             : toggle wireframe" << std::endl;

	float deltaTime = 0.0f;
	float fixedDeltaTime = 0.0f;
	double physicsElapsedTime = 0.0;

	Application application(deltaTime, fixedDeltaTime);	

	HWND hwnd = application.getWindow();
	HDC hdc;

	typedef std::chrono::milliseconds ms;
	Clock deltaClock;
	Clock fixedDeltaClock;

	// main message loop
	while (application.isRunning()) {


		physicsElapsedTime += deltaTime;
		while (physicsElapsedTime > PHYSICS_STEP) {
			fixedDeltaTime = fixedDeltaClock.restartSec();
			if (fixedDeltaTime > PHYSICS_STEP * 5.0f)
				fixedDeltaTime = PHYSICS_STEP;

			application.fixedUpdate();
			physicsElapsedTime -= PHYSICS_STEP;
		}

		application.update();		application.render();

		deltaTime = deltaClock.restartSec();
		
		hdc = GetDC(hwnd);
		SwapBuffers(hdc);
		ReleaseDC(hwnd, hdc);		
	} // end while

	hdc = GetDC(hwnd);
	wglMakeCurrent(hdc, 0);
	wglDeleteContext(wglGetCurrentContext());
	ReleaseDC(hwnd, hdc);

	return 0;
}